#ifndef WORD_MULTIPLEXER_H
#define WORD_MULTIPLEXER_H

namespace wordmodel {

  template<class ModelType>
  class ModelMultiplexer : WordModel {
    
    const std::string& get_prediction(int* prediction_id) { 
      double weights[model.size() + 1];
      
      //represent an empty model at the bottom
      weights[0] = 0;
      tokens_[0] = "";
      
      for(auto m : models_) {
	//use the token prediction if the model is not at an interface
	model[i + 1] = model.get_prediction(prediction_id, tokens_[i]);
	if(model.interface())
	  weights[i + 1] = weights[i] + model.get_prediction_weight(prediction_id);
	else
	  weight[i + 1] = model.get_prediction_weight(prediction_id);
      }
      
      return tokens_[model.size()];
 
    }

    bool putc(char c) {
      interface_ = false;
      for(auto m : models_) {
	m.interface(interface_);
	interface_ = m.putc(c);
      }
      
      return interface_;
    }
    void write_summary(std::ostream& out);

  private:
    std::vector<std::string> tokens_;
    std::vector<ModelType> models_;
    bool interface_;

  }

}

#endif // WORD_MULTIPLEXER_H
