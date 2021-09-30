#include "pid.h"
#define speed_output_incre_limit 100

float pid::get_slope_pid(float error, Car &car, float scalar){
    //Get now time
    struct timeval tv;
    gettimeofday( &tv, NULL );
    time_t tnow = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    time_t _last_tim = car.get_last_tim();
    int dt = tnow - _last_tim; // on ms
    float output = 0;
    
    float _last_derivative = car.get_last_derivative();
    if( _last_tim == 0 || dt > 1000 ) {
        dt = 0;
        // _integrator = 0;
        car.set_integrator(0);
        // _last_derivative = 0;
        car.set_last_derivative(0);
    }
    
    // _last_tim = tnow;
    car.set_last_tim(tnow);
    float delta_time = dt / 1000.0; // on s
    output += error * slope_p_;
    _last_derivative = car.get_last_derivative();
    float _last_error = car.get_last_error();  

    if ( abs(slope_d_) > 0 && dt > 0 ) {
        float derivative = 0;
        if ( _last_derivative == 0 ){
            derivative = 0;
            // _last_derivative = 0;
            car.set_last_derivative(0);
        }
        else{
            derivative = (error - _last_error) / delta_time;
        }
        _last_derivative = car.get_last_derivative();
        derivative = _last_derivative + (
                        (delta_time) / (rc_ + delta_time) * 
                        (derivative - _last_derivative)
                    );
        car.set_last_error(error);
        _last_error = car.get_last_error();
        // cout << "52_last_error: " << _last_error << endl;

        // _last_derivative = derivative;
        car.set_last_derivative(derivative);
        output += slope_d_ * derivative; 
    }
    output *= scalar;
    float _integrator = car.get_integrator();
    if( abs(slope_i_) > 0 && dt > 0 ) {
        // _integrator = _integrator + error;
        _integrator += ( error * slope_i_ ) * 
                scalar * delta_time;
 //       cout<<"_integrator1:"<<_integrator<<endl;
        if( _integrator < -500 ) 
            _integrator = -500;
        else if( _integrator > 500 )
            _integrator = 500;
        output += _integrator;
     // cout << "angel _output: " << output << endl;
     // cout << "integrator: " << _integrator << endl;
        car.set_integrator(_integrator);
    }
    // _integrator = car.get_integrator();
    // cout << "_integrator: " << _integrator << endl;
    // _last_error = car.get_last_error();
    // cout << "_last_error: " << _last_error << endl;
    // _last_derivative = car.get_last_derivative();
    // cout << "_last_derivative: " << _last_derivative << endl;
    // _last_tim = car.get_last_tim();
    // cout << "_last_tim: " << _last_tim << endl;
    return output;
}

float pid::get_speed_pid(float error, Car &car, float scalar){
    //Get now time
    struct timeval tv;
    gettimeofday( &tv, NULL );
    time_t tnow = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    time_t _last_tim = car.get_last_speed_tim();
    int dt = tnow - _last_tim; // on ms
    float output = 0;
    
    float _last_derivative = car.get_last_speed_derivative();
    if( _last_tim == 0 || dt > 1000 ) {
        dt = 0;
        // _integrator = 0;
        car.set_speed_integrator(0);
        // _last_derivative = 0;
        car.set_last_speed_derivative(0);
    }
    
    // _last_tim = tnow;
    car.set_last_speed_tim(tnow);
    float delta_time = dt / 1000.0; // on s
    output += error * speed_p_;
    _last_derivative = car.get_last_speed_derivative();
    float _last_error = car.get_last_speed_error();  

    if ( abs(speed_d_) > 0 && dt > 0 ) {
        float derivative = 0;
        if ( _last_derivative == 0 ){
            derivative = 0;
            // _last_derivative = 0;
            car.set_last_speed_derivative(0);
        }
        else{
            derivative = (error - _last_error) / delta_time;
        }
        _last_derivative = car.get_last_speed_derivative();
        derivative = _last_derivative + (
                        (delta_time) / (rc_ + delta_time) * 
                        (derivative - _last_derivative)
                    );
        car.set_last_speed_error(error);
        _last_error = car.get_last_speed_error();
        // cout << "52_last_error: " << _last_error << endl;

        // _last_derivative = derivative;
        car.set_last_speed_derivative(derivative);
        output += speed_d_ * derivative; 
    }
    output *= scalar;
    float _integrator = car.get_speed_integrator();
    if( abs(speed_i_) > 0 && dt > 0 ) {
        // _integrator = _integrator + error;
        _integrator += ( error * speed_i_ ) * 
                scalar * delta_time;
        //cout<<"_integrator1:"<<_integrator<<endl;
        if( _integrator < -400 ) 
            _integrator = -400;
        else if( _integrator > 400 )
            _integrator = 400;
         // cout<<"_integrator:"<<_integrator<<endl;
        output += _integrator;
        //if((output > 500) || (output<-500) )
          //  output = output - _integrator;
        if(output >0)
            output =0;
    // cout << "speed _output: " << output << endl;
        car.set_speed_integrator(_integrator);
    }
    // _integrator = car.get_integrator();
    // cout << "_integrator: " << _integrator << endl;
    // _last_error = car.get_last_error();
    // cout << "_last_error: " << _last_error << endl;
    // _last_derivative = car.get_last_derivative();
    // cout << "_last_derivative: " << _last_derivative << endl;
    // _last_tim = car.get_last_tim();
    // cout << "_last_tim: " << _last_tim << endl;
    return output;
}

float pid::get_speed_pid_incre(float error, Car &car, float scalar)
{
    //   PID计算输出增量
    float Speed_error1 = car.get_speed_error1();
    float Speed_error2 = car.get_speed_error2();
     // cout<<"move_error:"<<error<<endl;
     float speed_intergrator=speed_i_ * error;
     if (speed_intergrator<-50)
        speed_intergrator=-50;
     else if(speed_intergrator>50)
        speed_intergrator=50;

    float Speed_output_increment = (speed_p_ *(error - Speed_error1) 
        + speed_intergrator + speed_d_ * (error + Speed_error2 - 
            2 * Speed_error1));
    car.set_speed_error2(Speed_error1);
    car.set_speed_error1(error);
    // Speed_error2 = Speed_error1;
    Speed_error1 = car.get_speed_error1();
    Speed_error2 = car.get_speed_error2();
    // cout << "Speed_error1: " << Speed_error1 << endl;
    // cout << "Speed_error2: " << Speed_error2 << endl;  
    // Speed_error1 = error;
   // cout << "Speed_output_increment: " << Speed_output_increment << endl; 
    
    //输出速度增量限幅，防止速度过快出现过冲等现象，具体的speed_output_limit需要根据实际调节
   //if( Speed_output_increment < -speed_output_incre_limit)
     // Speed_output_increment = -speed_output_incre_limit;
   //else if(Speed_output_increment > speed_output_incre_limit)
       //Speed_output_increment = speed_output_incre_limit;
    
    //更新本次需要输出的速度量
    float Speed_output = car.get_speed_output();
    //cout << "Speed_output: " << Speed_output << endl;
    Speed_output = Speed_output + Speed_output_increment;
    
    //对输出的速度进行限幅，防止越限
      if   (Speed_output > 0.0)
          Speed_output = 0;
      else if(Speed_output < -500)
          Speed_output = -500;
    car.set_speed_output(Speed_output);
    Speed_output = car.get_speed_output();
    // cout << "Speed_output: " << Speed_output << endl;
    
    return Speed_output;    
}

/**
 *
 * control car through targets of speed and angular
 * input: target speed, angular, current speed, 
 * angular, car
 * 
**/
void pid::controlSpeedAndAngular(Car &car)
{
    float target_slope = car.get_target_slope();
    if (target_slope > 180)
    {
        target_slope = target_slope-360;
    }
    float current_slope = car.get_slope();
    float error_angel = current_slope - target_slope;
     
    if( error_angel < -180)
        error_angel = error_angel+360;
    else if( error_angel > 180)
        error_angel = error_angel-360;
    slope_p_ = car.get_slope_p();
    slope_i_ = car.get_slope_i();
    slope_d_ = car.get_slope_d();
    // cout << "slope_p: " << slope_p_ << endl;
    // cout << "slope_i: " << slope_i_ << endl;
    // cout << "slope_d: " << slope_d_ << endl;

    // update parameter
    pid pid_turn;
    pid_turn.set_slope_pid(slope_p_, slope_i_, slope_d_);
 //    if (abs(error_angel) > 5)
 //        pid_turn.set_pid(param_turn_p - 0.4, param_turn_i, 
 //           param_turn_d + 0.02);   
      // cout << "error_angel: " << error_angel << endl; 
    wp_angel_ = pid_turn.get_slope_pid(error_angel, car, 100);
    if (wp_angel_ > 2000)
        wp_angel_ = 2000;
    else if (wp_angel_ < -2000)
        wp_angel_ = -2000;
    float target_speed = car.get_target_speed();
    float currentSpeed = car.get_speed();
    float error_move=target_speed - currentSpeed;
     // cout << "error_move: " << error_move << endl;
    // set a minimax value
   //if (abs(error_move) < 1) return;
    // cout << "currentSpeed: " << currentSpeed << endl;
    // Normalized on specifical value, here is 1m
  //  error_move /= 1000;
    error_move = (abs(error_move) > 1000 )? 1:error_move;
    
    // speed pid
    speed_p_ = car.get_speed_p();
    speed_i_ = car.get_speed_i();
    speed_d_ = car.get_speed_d();

    pid pid_move;   
    pid_move.set_speed_pid(speed_p_, speed_i_, speed_d_);
    // wp_move_ = pid_move.get_speed_pid_incre(error_move, 
    //         car, 100);
    wp_move_ = pid_move.get_speed_pid(error_move, car, 100);
    if(wp_move_<-600.0)
        wp_move_=-600;
    else if (wp_move_>600)
    {
        wp_move_=600;/* code */
    }
    if (target_speed == 0)
        wp_move_ = 0;
    if (car.get_marker() == 1)
    {
        cout<<"wp_move_"<<wp_move_<<endl;
        cout << "target_speed: " << target_speed << endl;
    }
     
    //wp_move_=-500;
    float duty_left = -wp_angel_/2 + (int)wp_move_; 
    duty_left *= -1;
    float duty_right = wp_angel_/2 + (int)wp_move_;
    /*
//防止输出越限，在左右输出减去相同的量。即保证转向角度控制，弱化速度控制。代码需测试 
//    if(duty_left<-900.0)
 //   {
 //       duty_left=-900.0;
        duty_right=duty_right-900-duty_left;
    }
    else if(duty_left>900.0)    
    {
        duty_left=900;
        duty_right=duty_right+900-duty_left;
    }
    else if (duty_right<-900.0)
    {
        duty_right=-900.0;
        duty_left=duty_left-900-duty_right;
    }
    else if(duty_right>900.0)
    {
        duty_right=900.0;
        duty_left=duty_left+900-duty_right;
    }
*/
    /* define comm format*/
    char a[6] = {0x11,0x00,0x00,0x00,0x00,0x22};
    // combinate the corresponding command
    a[1] = (unsigned char)((((short)duty_left)>>8) & 0xff);
    a[2] = (unsigned char)(((short)duty_left) & 0xff);
    a[3] = (unsigned char)((((short)duty_right)>>8) & 0xff);
    a[4] = (unsigned char)(((short)duty_right) & 0xff);
    // build communication based on udp
    string ip = car.get_ip();
    udp udp_comm;
    int sock_fd = udp_comm.udp_init(ip, 10000);
    // stop car in emergency
    int stop_flag = (int)car.get_stop_flag();
    if (stop_flag)
    {
        a[1] = 0x00;
        a[2] = 0x00;
        a[3] = 0x00;
        a[4] = 0x00;
    }
    // send data through udp
    udp_comm.send_data(sock_fd, a, sizeof(a));
}