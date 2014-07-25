#ifndef PACKET_H
#define PACKET_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

class Packet
{
public:

  enum PACKET_TYPE {TRAIN, PREDICT, PREDICTION};
  const char* PACKET_STRINGS[3] = {"Train", "Predict", "Prediction"};
  enum { header_length = 4 + 1 };
  enum { max_body_length = 512 };

  Packet()
    : body_length_(0), type_(TRAIN)
  {
  }

  PACKET_TYPE type() const
  {
    return type_;
  }

  const char* type_string() const
  {
    return PACKET_STRINGS[type_];
  }


  void type(PACKET_TYPE type)
  {
    type_ = type;
  }


  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    std::sscanf(header, "%04d%01d", &body_length_,&type_);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[header_length + 1] = "";
    std::sprintf(header, "%04d%01d", body_length_,type_);
    std::memcpy(data_, header, header_length);
  }

private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
  PACKET_TYPE type_;
};


#endif //PACKET_H
