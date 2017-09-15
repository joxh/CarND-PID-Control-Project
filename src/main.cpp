#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <fstream>


// for convenience
using json = nlohmann::json;

using namespace std;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  PID pid;
  PID pid_speed;
  // TODO: Initialize the pid variable.

  


  bool there_is_a_json_init_file = false; // The build time flag for whether to use json
  bool use_json_init_file;
  json json_init; //This package is used in main, so I think it's okay
  if (there_is_a_json_init_file){
    ifstream json_init_file("params/init.json");
    json_init_file >> json_init;
    use_json_init_file = json_init["use_json_init_file"];
  } else {
    use_json_init_file = false;
  }

  double Kp, Ki, Kd;
  double Kp_speed, Ki_speed, Kd_speed;
  double fixed_throttle;
  double target_speed;
  if (use_json_init_file){
    Kp = json_init["Kp"];
    Ki = json_init["Ki"];
    Kd = json_init["Kd"];
    Kp_speed = json_init["Kp_speed"];
    Ki_speed = json_init["Ki_speed"];
    Kd_speed = json_init["Kd_speed"];
    target_speed = json_init["target_speed"];
    fixed_throttle = json_init["fixed_throttle"];
    
  } else {
    Kp = 0.18;
    Ki = 8.0e-6;
    Kd = 3.00;
    Kp_speed = 0.01;
    Ki_speed = 6.0e-6;
    Kd_speed = 0.00;
    target_speed = 40.0;
    fixed_throttle = .3;
  }

  pid.Init(Kp, Ki, Kd);
  pid_speed.Init(Kp_speed, Ki_speed, Kd_speed);


  h.onMessage([&pid, &pid_speed, fixed_throttle, target_speed](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */
          double target_speed_to_use;
          bool live_json = false;
          if (live_json){
            ifstream json_init_file("params/init.json");
            json json_init;
            json_init_file >> json_init;


            pid.Kp = json_init["Kp"];
            pid.Ki = json_init["Ki"];
            pid.Kd = json_init["Kd"];
            pid_speed.Kp = json_init["Kp_speed"];
            pid_speed.Ki = json_init["Ki_speed"];
            pid_speed.Kd = json_init["Kd_speed"];
            target_speed_to_use = json_init["target_speed"];
            
          } else {
            target_speed_to_use = target_speed;
          }
          

          pid.UpdateError(cte);
          steer_value = - pid.Kp * pid.p_error - pid.Ki * pid.i_error - pid.Kd * pid.d_error;
          steer_value =  (steer_value < -1.0) ? -1.0 : steer_value;
          steer_value =  steer_value > 1.0 ? 1.0 : steer_value;
          
          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          pid_speed.UpdateError(speed-target_speed_to_use);

          double throttle_value ;
          throttle_value= - pid_speed.Kp * pid_speed.p_error - pid_speed.Ki * pid_speed.i_error - pid_speed.Kd * pid_speed.d_error;

          throttle_value =  (throttle_value < -0.1) ? -0.1 : throttle_value;
          throttle_value =  throttle_value > 1.0 ? 1.0 : throttle_value;
          
          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle_value;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
