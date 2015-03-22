#ifndef CONTEXT_TREE_H
#define CONTEXT_TREE_H

#include <functional>
#include <deque>
#include <vector>
#include <unordered_map>
#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/graph_traits.hpp> 
#include <boost/tuple/tuple.hpp> 
#include <boost/graph/graphviz.hpp>
#include <iostream>
#include <cassert>
#include "btree/btree_map.h"

#define MAX_DEGREE_DEFAULT 10

namespace wordmodel {

  /** \brief A context tree implemented in Boost Graph Library
   *
   * 
   */

  template<class M, typename DataType, typename ContextData, int max_degree=MAX_DEGREE_DEFAULT >
  class ContextTree {

    static_assert(max_degree >= 2, "Max degree must be greater than 1");

    enum {root_token = 0 };
    enum {reserved_tokens = 1 };

    struct VertexProperty;
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, VertexProperty> Graph;
    typedef typename Graph::vertex_descriptor Vertex;


    typedef btree::btree_map<size_t, Vertex> Subtree;
    
    struct VertexProperty{
      //unique for each node
      size_t id; 
      //data index in node
      size_t token;
      //subtree, added for performance reasons
      Subtree* subtree;
      VertexProperty() : id(0), token(0), subtree(NULL) {
      }

    };
      
    typedef typename Graph::edge_descriptor Edge;
    typedef typename boost::graph_traits < Graph >::adjacency_iterator VertexIter;

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
				       max_contexts_(max_contexts) {     
      
      root_ = boost::add_vertex(tree_);
      tree_[root_].id = vertex_number_++;
      tree_[root_].token = root_token;
      token_number_++;

      contexts_ = new Context[max_contexts];
      for(unsigned int i = 0; i < max_contexts; i++)
	inactive_ids_.push_back(i);
    }
    
    ContextTree(ContextTree&& other) : tree_(std::move(other.tree_)),
				       visitor(other.visitor),
				       root_(other.root_),
				       vertex_number_(other.vertex_number_),
				       token_number_(other.token_number_),
				       max_contexts_(other.max_contexts_),
				       data_list_(std::move(other.data_list_)),
				       data_map_(std::move(other.data_map_)),
				       contexts_(other.contexts_),
				       active_ids_(std::move(other.active_ids_)),
				       inactive_ids_(std::move(other.inactive_ids_)){

      other.visitor = NULL;
      other.contexts_ = NULL;
      for (auto vp = boost::vertices(other.tree_); 
	   vp.first != vp.second; 
	   ++vp.first)
	other.tree_[*vp.first].subtree = NULL;

    }
    
    ~ContextTree(){
      if(contexts_ != NULL)
	delete[] contexts_;
      for (auto vp = boost::vertices(tree_); 
	   vp.first != vp.second; 
	   ++vp.first) {	
	if(tree_[*vp.first].subtree)
	  delete tree_[*vp.first].subtree;
      }
      
    }

    void set_visitor(M* v) {
      visitor = v;
    } 

    template<typename A>
    void predict(A data_frag, size_t length, unsigned int* context_id) {

      Context& con = activate_context(context_id);


#ifdef DEBUG_CT
      std::cout << "CT Prediction for context_id <" << *context_id << ">" << std::endl;
#endif


      //make sure context is big enough
      //add 1 to make room for empty node/root node
      if(con.path.size() < length + 1)
	con.path.resize(length + 1);      
      
      visitor->start_predict(con.data);

      //descend through root
      Vertex v = root_;      
      bool found_node;
      for(unsigned int i = 0;; ++i) {

	//treat the ith vertex
	//store the path we're taking
	con.path[i].id = tree_[v].id;
	//call visitor on vertex
	visitor->push_predict(tree_[v].id, i, con.data);

	if(i == length)
	  break;

	//now find the next vertex
	DataType c = data_frag[length - i - 1];

	//check if it's a new token
	auto it = data_map_.find(c);
	if(it == data_map_.end()) {
	  data_map_[c] = token_number_++;
	  data_list_.emplace_back(std::move(c));
	  found_node = false;
	} else {
	  //get next node, possibly going through btree
	  found_node = descend(&v, (*it).second);
	}

	//is v null -> doesn't exist
	if(!found_node) {
#ifdef DEBUG_CT
	  std::cout << "Finished on round " << i << std::endl;
#endif
	  con.leaf = v;
	  // Now we process the additions we'd like to make
	  // if our prediction fails
	  for(unsigned int j = i+1; j < length+1; ++j) {
#ifdef DEBUG_CT
	    std::cout << "I want to add a " << data_map_[c] << " node to chain starting at " << con.path[i].id << " at a depth of " << j - i << std::endl;
#endif
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

    void regret(unsigned int context_id, int depth) {

#ifdef DEBUG_CT
      std::cout << "CT Regret for context_id <" << context_id << ">" << std::endl;
#endif

      if(!validate_id(context_id))
	return;
      
#ifdef DEBUG_CT
      std::cout << "CT Regret for context_id <" << context_id << ">: proceeding, due to valid id" << std::endl;
#endif

	  
      //get context
      Context& con = contexts_[context_id];      
      
      //move down through to the new vertices 
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
      con_add_nodes(con, index + 1, depth, true);                 
      deactivate_context(context_id);
    }

    void reinforce(unsigned int context_id, int depth) {

#ifdef DEBUG_CT
      std::cout << "CT Reinforce for context_id <" << context_id << ">" << std::endl;
#endif

      if(!validate_id(context_id))
	return;
      
#ifdef DEBUG_CT
      std::cout << "CT Reinforce for context_id <" << context_id << ">: proceeding, due to valid id" << std::endl;
#endif

	  
      //get context
      Context& con = contexts_[context_id];      
      
      //move down through to the new vertices 
      int index = 0;
      for(;con.path[index].id != tree_[con.leaf].id; index++) {
	visitor->push_reinforce(con.path[index].id, 
			     index,
			     con.data);			     
      }

      //now push the leaf
      visitor->push_reinforce(con.path[index].id, 
			   index,
			   con.data);			     
      con_add_nodes(con, index + 1, depth, false);                 
      deactivate_context(context_id);
    }

    /** \brief Write graphviz output to the given stream
     * 
     * The token_names should be a container of strings.
     *
     */
    template<typename T>
    void write_graph(std::ostream& out, const T& token_names) {
      //construct names
      const char* names[vertex_number_];      
      Vertex v;
      int i = 0;      
      std::vector<Vertex> temp_v;      
      for (auto vp = boost::vertices(tree_); 
	   vp.first != vp.second; 
	   ++vp.first) {	
	v = *vp.first;
	names[i] = token_names[tree_[v].token + reserved_tokens].c_str();

	//check if we need to add temperorary edges
	if(tree_[v].subtree) {

	  for(auto sit : *(tree_[v].subtree)) {
	    boost::add_edge(v,sit.second,tree_);
	    temp_v.push_back(v);
	  }
	}	
	i++;			       
      }
            
      boost::write_graphviz(out, tree_, boost::make_label_writer(names));

      //remove temp edges
      for(auto v : temp_v) {
	for(auto sit : *(tree_[v].subtree)) {
	  boost::remove_edge(v, sit.second, tree_);
	}
      }
    }
    
    
  private:

    inline
    size_t hash(size_t token, int key) const {
      //taken from boost::hash_combine
      return key^hash_fn_(hash_fn_(token) + 0x9e3779b9 + (key << 6) + (key >> 2));
    }

    void con_add_nodes(Context& con, int index, int depth, bool regret) {

      Vertex& last = con.leaf;
	
      //add nodes
      for(int i = index; i < depth && i < con.addition_size + index; ++i) {
#ifdef DEBUG_CT
	std::cout << "About to add " << vertex_number_
		  << ", a " << con.path[i].token 
		  << " node, to " << tree_[last].id << std::endl;
#endif

	Vertex v = boost::add_vertex(tree_);
	tree_[v].id = vertex_number_++;
	tree_[v].token = con.path[i].token;

	//check if we should be using a subtree
	if(!tree_[last].subtree && boost::out_degree(last,tree_) == max_degree) {
	  //split it up
	  split_vertex(last);
	}

	//check if we're using a subtree
	if(tree_[last].subtree) {
	  //simple
	  tree_[last].subtree->
	    insert(std::make_pair(con.path[i].token,v));
	  
	}  else {
	  //add normally
	  boost::add_edge(last, v, tree_);
	}
	last = v;
	visitor->add_node(tree_[v].id, index, con.data, regret);
      }


    }


    bool validate_id(unsigned int context_id) {
      //check if id is valid
      if(context_id < 0 || context_id > max_contexts_)
	return false;
      
      //check if it's still valid
      for(auto i: inactive_ids_)
	if(i == context_id)
	  return false;	
      
      return true;
    }

    //put the context back in the inactive set
    void deactivate_context(unsigned int context_id) {
      for(auto it = active_ids_.begin(); it != active_ids_.end(); ++it) {
	if(*it == context_id) {
	  active_ids_.erase(it);
	  break;
	}
      }
      inactive_ids_.push_back(context_id);
    }

    void flush_ids() {
      //flush half of the contexts
      while(2 * inactive_ids_.size() < max_contexts_) {
	inactive_ids_.push_back(active_ids_.front());
	active_ids_.pop_front();	
      }
    }

    Context& activate_context(unsigned int* context_id) {
      //get a free context_id
      if(inactive_ids_.empty())
	flush_ids();
      
      *context_id = inactive_ids_.front();      
      inactive_ids_.pop_front();      
      active_ids_.push_back(*context_id);

      //initialize context
      Context& con = contexts_[*context_id];
      con.addition_size = 0;
      con.leaf = root_;

      return con;
    }


    /**
     * This is used when we split up a parent-children relationship
     * into a subtree.  Currently the child vertices are not deleted,
     * which I'm not sure is the correct decision.
     **/
    void split_vertex(Vertex& v) {

      //set-up v to more children
      tree_[v].subtree = new Subtree;

#ifdef DEBUG_CT
      std::cout << "Splitting " << tree_[v].id << " into " 
		<< max_degree << std::endl;
#endif

      VertexIter target, end;
      Vertex to_delete[max_degree];
      int i = 0;
      for(tie(target,end) = boost::adjacent_vertices(v,tree_); 
	  target != end;
	  ++target) {
	tree_[v].subtree->insert(std::make_pair(tree_[*target].token,
					      *target));
	to_delete[i++] = *target;
      }      

      for(i = 0; i < max_degree; i++)
	boost::remove_edge(v,*target,tree_);
      
    }


    /* This will descend through any number of hashes and return
     * the parent node corresponding to the given token
     *
     *
     */
    inline
    bool descend(Vertex* v, size_t token) const{

      Subtree* subtree = tree_[*v].subtree;      

      if(subtree != NULL) {
	auto lookup = subtree->find(token);
	if(lookup == subtree->end())
	  return false;
	*v = lookup->second;
	return true;
      }
      
      VertexIter target, end;       
      for(tie(target,end) = boost::adjacent_vertices(*v,tree_); 
	  target != end;
	  ++target) {
	if(tree_[*target].token == token) {
	  *v = *target;
	  return true;
	}
      }
      
      return false;
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
    std::hash<size_t> hash_fn_;
  };
}

#endif //CONTEXT_TREE_H
