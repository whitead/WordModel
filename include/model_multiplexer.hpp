#ifndef WORD_MULTIPLEXER_H
#define WORD_MULTIPLEXER_H

namespace wordmodel {

  template<class ModelType>
  class ModelMultiplexer : public WordModel {
  public:
    
    ModelMultiplexer(int levels) {
      for(int i = 0; i < levels; i++)
	models_.push_back( ModelType() );
    }

    using WordModel::get_prediction;
    
    const std::string& get_prediction(unsigned int* prediction_id) override{ 
      int i = 0;
      while(i < models_.size() - 1) {
	//go up higher as long as we're at an interface
	if(models_[i].detected_interface())
	  i++;
	else
	  break;
      }
      
      return models_[i].get_prediction(prediction_id);
 
    }

    bool putc(char c) override{

      bool temp;

      //treat the lowest level specially
      interface_ = models_[0].putc(c);
      models_[0].interface(true);

      for(int i = 1; i < models_.size(); i++) {
	if(!interface_)
	  interface_ = models_[i].putc(c);
	else
	  models_[i].interface(interface_);	
      }
      
      return interface_;
    }

    void write_summary(std::ostream& out) override {
      out << "A model multiplexer containing models from smallest token size to largest" << std::endl;
      for(auto &m: models_)
	m.write_summary(out);      
    }

    long int training_mistakes() const override {
      long int sum = 0;
      for(auto &m: models_)
	sum += m.training_mistakes();
      return sum;

    }

  private:
    std::vector<std::string> tokens_;
    std::vector<ModelType> models_;
    bool interface_;

  };

}

#endif // WORD_MULTIPLEXER_H
