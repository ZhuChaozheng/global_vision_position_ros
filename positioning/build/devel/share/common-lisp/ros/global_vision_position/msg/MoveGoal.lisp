; Auto-generated. Do not edit!


(cl:in-package global_vision_position-msg)


;//! \htmlinclude MoveGoal.msg.html

(cl:defclass <MoveGoal> (roslisp-msg-protocol:ros-message)
  ((car_target_x
    :reader car_target_x
    :initarg :car_target_x
    :type cl:float
    :initform 0.0)
   (car_target_y
    :reader car_target_y
    :initarg :car_target_y
    :type cl:float
    :initform 0.0)
   (car_target_theta
    :reader car_target_theta
    :initarg :car_target_theta
    :type cl:float
    :initform 0.0))
)

(cl:defclass MoveGoal (<MoveGoal>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <MoveGoal>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'MoveGoal)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name global_vision_position-msg:<MoveGoal> is deprecated: use global_vision_position-msg:MoveGoal instead.")))

(cl:ensure-generic-function 'car_target_x-val :lambda-list '(m))
(cl:defmethod car_target_x-val ((m <MoveGoal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader global_vision_position-msg:car_target_x-val is deprecated.  Use global_vision_position-msg:car_target_x instead.")
  (car_target_x m))

(cl:ensure-generic-function 'car_target_y-val :lambda-list '(m))
(cl:defmethod car_target_y-val ((m <MoveGoal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader global_vision_position-msg:car_target_y-val is deprecated.  Use global_vision_position-msg:car_target_y instead.")
  (car_target_y m))

(cl:ensure-generic-function 'car_target_theta-val :lambda-list '(m))
(cl:defmethod car_target_theta-val ((m <MoveGoal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader global_vision_position-msg:car_target_theta-val is deprecated.  Use global_vision_position-msg:car_target_theta instead.")
  (car_target_theta m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <MoveGoal>) ostream)
  "Serializes a message object of type '<MoveGoal>"
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'car_target_x))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'car_target_y))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'car_target_theta))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <MoveGoal>) istream)
  "Deserializes a message object of type '<MoveGoal>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'car_target_x) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'car_target_y) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'car_target_theta) (roslisp-utils:decode-double-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<MoveGoal>)))
  "Returns string type for a message object of type '<MoveGoal>"
  "global_vision_position/MoveGoal")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'MoveGoal)))
  "Returns string type for a message object of type 'MoveGoal"
  "global_vision_position/MoveGoal")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<MoveGoal>)))
  "Returns md5sum for a message object of type '<MoveGoal>"
  "0746615679f21dee670288dd78bf0cea")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'MoveGoal)))
  "Returns md5sum for a message object of type 'MoveGoal"
  "0746615679f21dee670288dd78bf0cea")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<MoveGoal>)))
  "Returns full string definition for message of type '<MoveGoal>"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the goal~%float64 car_target_x  # Specify car's　target position~%float64 car_target_y~%float64 car_target_theta~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'MoveGoal)))
  "Returns full string definition for message of type 'MoveGoal"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the goal~%float64 car_target_x  # Specify car's　target position~%float64 car_target_y~%float64 car_target_theta~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <MoveGoal>))
  (cl:+ 0
     8
     8
     8
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <MoveGoal>))
  "Converts a ROS message object to a list"
  (cl:list 'MoveGoal
    (cl:cons ':car_target_x (car_target_x msg))
    (cl:cons ':car_target_y (car_target_y msg))
    (cl:cons ':car_target_theta (car_target_theta msg))
))
