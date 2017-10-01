/* mumble.cpp */

#include "mumble.h"
#include "callback.h"
#include "utils.h"
#include "os/thread.h"
#include "print_string.h"
#include <mumlib/Transport.hpp>
#include <string>
#include "scene/main/timer.h"


void Mumble::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add", "value"), &Mumble::add);
    ClassDB::bind_method(D_METHOD("reset"), &Mumble::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Mumble::get_total);

   ClassDB::bind_method(D_METHOD("connect", "host", "port", "user", "password"), &Mumble::connect);
   ClassDB::bind_method(D_METHOD("setCallback", "callback"), &Mumble::setCallback);
}
void Mumble::connect(String host, int port, String user, String password){
   std::string h = utils::gstr2cpp_str(host);
   std::string u = utils::gstr2cpp_str(user);
   std::string p = utils::gstr2cpp_str(password);
   while(true){
   try{
      this->mum->connect(h, port,  u, p);
      this->mum->run();
   }catch (mumlib::TransportException &exp) {
      print_line( "Mumble: error " + utils::cpp_str2gstr(exp.what()));
      print_line( "Mumble: attempting to reconnect in 5s. ");
      Timer * sleep = memnew(Timer);
      sleep -> set_wait_time(5.0);
      sleep -> start();
      memdelete(sleep);
      
   }
   }
}
void Mumble::setCallback(Object * callback){
   SimpleCallback *cb = Object::cast_to<SimpleCallback>(callback);
   mum = new mumlib::Mumlib(*cb);
}


Mumble::Mumble() {
    count=0;
}

void Mumble::add(int value) {

    count+=value;
}

void Mumble::reset() {

    count=0;
}


int Mumble::get_total() const {

    return count;
}
