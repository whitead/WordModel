#ifndef CONTEXT_TREE_H
#define CONTEXT_TREE_H

#include <deque>
#include <vector>
#include <unordered_map>
#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/graph_traits.hpp> 
#include <boost/tuple/tuple.hpp> 
#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace wordmodel {

  template<class M, typename DataType, typename ContextData>
  class ContextTree {
    
    struct VertexProperty{
      size_t id;
      size_t token;
    };
      
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, VertexProperty> Graph;
    typedef typename Graph::vertex_descriptor Vertex;
    typedef typename boost::graph_traits < Graph >::adjacency_iterator V_iter;

    struct Context {
      Vertex leaf;
      std::vector<VertexProperty> path;
      int addition_size;
      ContextData data;
    };
      
  public:
    ContextTree(size_t max_contexts) : tree_(0),
				       vertex_number_(0),
				       token_number_(0),
				       max_contexts_(max_contexts){     
      
      root_ = boost::add_vertex(tree_);
      tree_[root_].id = vertex_number_++;
      tree_[root_].token = token_number_++;

      contexts_ = new Context[max_contexts];
      for(int i = 0; i < max_contexts; i++)
	inactive_ids_.push_back(i);
    }

    ~ContextTree(){
      delete[] contexts_;
    }

    void set_visitor(M* v) {
      visitor = v;
    } 

    template<typename A>
    void predict(A data_frag, size_t length, int* context_id) {

      Context& con = activate_context(context_id);

      //make sure context is big enough
      //add 1 to make room for empty node/root node
      if(con.path.size() < length + 1)
	con.path.resize(length + 1);      
      
      visitor->start_predict(con.data);

      Vertex v = root_;
      V_iter target, end;
      bool terminate;

      for(int i = 0;; ++i) {

	//treat the ith vertex
	//store the path we're taking
	con.path[i].id = tree_[v].id;
	//call visitor on vertex
	visitor->push_predict(tree_[v].id, i, con.data);

	if(i == length)
	  break;

	//now find the next vertex
	DataType c = data_frag[length - i - 1];
	terminate = true;
	
	//check if we are at a leaf by
	//iterating over edges
	for(tie(target,end) = boost::adjacent_vertices(v,tree_); 
	    target != end;
	    ++target) {
	  if(tree_[*(target)].token < token_number_ &&
	     c == data_list_[tree_[*(target)].token]) {
	    v = *target;	  
	    terminate = false;
	  }
	}

	if(terminate) {
	  con.leaf = v;
	  // Now we process the additions we'd like to make
	  // if our prediction fails
	  for(int j = i+1; j < length+1; ++j) {
	    //check if it's a new token
	    auto it = data_map_.find(c);
	    if(it == data_map_.end()) {
	      data_map_[c] = token_number_++;
	      data_list_.emplace_back(std::move(c));	      
	    }
	    con.path[j].token = data_map_[c];
	    con.addition_size++;
	    
	    //get new token
	    if(j < length)
	      c = data_frag[length - j - 1];
	  }
	  break;
	}
      }
      
      visitor->finish_predict(con.data);      
      
    }

    void regret(int context_id, int depth) {
      
      //check if id is valid
      if(context_id < 0 || context_id > max_contexts_)
	return;
      
      //check if it's still valid
      for(auto i: inactive_ids_)
	if(i == context_id)
	  return;	


	  
      //get context
      Context& con = contexts_[context_id];      
      
      //move down to the new vertices 
      int index = 0;
      for(;con.path[index].id != tree_[con.leaf].id; index++) {
	visitor->push_regret(con.path[index].id, 
			     index,
			     con.data);			     
      }

      //now push the leaf
      visitor->push_regret(con.path[index].id, 
			   index,
			   con.data);			     
      index++;

      

      Vertex last = con.leaf;
	
      //add nodes
      for(int i = index; i < depth && i < con.addition_size + index; ++i) {
	Vertex v = boost::add_vertex(tree_);	  
	tree_[v].id = vertex_number_++;
	tree_[v].token = con.path[i].token;
	visitor->add_node(tree_[v].id, index, con.data);
	boost::add_edge(last, v, tree_);
	last = v;
      }
      
      
      deactivate_context(context_id);
    }

    /** \brief Write graphviz output to the given stream
     * 
     * The token_names should be a container of strings.
     *
     */
    template<typename T>
    void write_graph(std::ostream& out, const T& token_names) const {
      //construct names
      const char* names[vertex_number_];      
      int i = 0;      
      for (auto vp = boost::vertices(tree_); 
	   vp.first != vp.second; 
	   ++vp.first) {	
	names[i] = token_names[tree_[*vp.first].token].c_str();
	i++;			       
      }
      
      
      boost::write_graphviz(out, tree_, boost::make_label_writer(names));
    }
    
    
  private:

    //put the context back in the inactive set
    void deactivate_context(int context_id) {
      for(auto it = active_ids_.begin(); it != active_ids_.end(); ++it) {
	if(*it == context_id) {
	  active_ids_.erase(it);
	  break;
	}
      }
      inactive_ids_.push_back(context_id);
    }

    void flush_ids_() {
      //flush half of the contexts
      while(2 * inactive_ids_.size() < max_contexts_) {
	inactive_ids_.push_back(active_ids_.front());
	active_ids_.pop_front();	
      }
    }

    Context& activate_context(int* context_id) {
      //get a free context_id
      if(inactive_ids_.empty())
	flush_ids_();
      
      *context_id = inactive_ids_.front();      
      inactive_ids_.pop_front();      
      active_ids_.push_back(*context_id);

      //initialize context
      Context& con = contexts_[*context_id];
      con.addition_size = 0;
      con.leaf = root_;
      return con;
    }
    
    Graph tree_;
    M* visitor;
    Vertex root_;
    size_t vertex_number_;
    size_t token_number_;
    size_t max_contexts_;
    //goes from index to data
    std::vector<DataType> data_list_; 
    //goes from data to indices
    std::unordered_map<DataType, size_t> data_map_;
    Context* contexts_;
    std::deque<size_t> active_ids_;
    std::deque<size_t> inactive_ids_;           
    
  };
}


#endif //CONTEXT_TREE_H
