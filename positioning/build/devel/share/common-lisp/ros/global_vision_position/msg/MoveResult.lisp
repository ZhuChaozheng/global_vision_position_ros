; Auto-generated. Do not edit!


(cl:in-package global_vision_position-msg)


;//! \htmlinclude MoveResult.msg.html

(cl:defclass <MoveResult> (roslisp-msg-protocol:ros-message)
  ((car_final_x
    :reader car_final_x
    :initarg :car_final_x
    :type cl:float
    :initform 0.0)
   (car_final_y
    :reader car_final_y
    :initarg :car_final_y
    :type cl:float
    :initform 0.0)
   (car_final_theta
    :reader car_final_theta
    :initarg :car_final_theta
    :type cl:float
    :initform 0.0))
)

(cl:defclass MoveResult (<MoveResult>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <MoveResult>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'MoveResult)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name global_vision_position-msg:<MoveResult> is deprecated: use global_vision_position-msg:MoveResult instead.")))

(cl:ensure-generic-function 'car_final_x-val :lambda-list '(m))
(cl:defmethod car_final_x-val ((m <MoveResult>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader global_vision_position-msg:car_final_x-val is deprecated.  Use global_vision_position-msg:car_final_x instead.")
  (car_final_x m))

(cl:ensure-generic-function 'car_final_y-val :lambda-list '(m))
(cl:defmethod car_final_y-val ((m <MoveResult>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader global_vision_position-msg:car_final_y-val is deprecated.  Use global_vision_position-msg:car_final_y instead.")
  (car_final_y m))

(cl:ensure-generic-function 'car_final_theta-val :lambda-list '(m))
(cl:defmethod car_final_theta-val ((m <MoveResult>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader global_vision_position-msg:car_final_theta-val is deprecated.  Use global_vision_position-msg:car_final_theta instead.")
  (car_final_theta m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <MoveResult>) ostream)
  "Serializes a message object of type '<MoveResult>"
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'car_final_x))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'car_final_y))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'car_final_theta))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <MoveResult>) istream)
  "Deserializes a message object of type '<MoveResult>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'car_final_x) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'car_final_y) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'car_final_theta) (roslisp-utils:decode-double-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<MoveResult>)))
  "Returns string type for a message object of type '<MoveResult>"
  "global_vision_position/MoveResult")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'MoveResult)))
  "Returns string type for a message object of type 'MoveResult"
  "global_vision_position/MoveResult")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<MoveResult>)))
  "Returns md5sum for a message object of type '<MoveResult>"
  "e38af6ac246dae4cfc91e60fb6a21bf6")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'MoveResult)))
  "Returns md5sum for a message object of type 'MoveResult"
  "e38af6ac246dae4cfc91e60fb6a21bf6")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<MoveResult>)))
  "Returns full string definition for message of type '<MoveResult>"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the result~%float64 car_final_x~%float64 car_final_y~%float64 car_final_theta~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'MoveResult)))
  "Returns full string definition for message of type 'MoveResult"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the result~%float64 car_final_x~%float64 car_final_y~%float64 car_final_theta~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <MoveResult>))
  (cl:+ 0
     8
     8
     8
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <MoveResult>))
  "Converts a ROS message object to a list"
  (cl:list 'MoveResult
    (cl:cons ':car_final_x (car_final_x msg))
    (cl:cons ':car_final_y (car_final_y msg))
    (cl:cons ':car_final_theta (car_final_theta msg))
))
