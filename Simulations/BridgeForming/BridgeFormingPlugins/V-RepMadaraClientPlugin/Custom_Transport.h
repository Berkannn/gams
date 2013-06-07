/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _CUSTOM_TRANSPORT_H_
#define _CUSTOM_TRANSPORT_H_

/**
 * @file Custom_Transport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Custom_Transport class, which provides a
 * multicast transport for sending knowledge updates in KaRL
 **/

#include <winsock2.h>
#include <string>

#include "madara/transport/Transport.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"

#include "Custom_Transport_Read_Thread.h"

/**
  * @class Custom_Transport
  * @brief Multicast-based transport for knowledge
  **/
class Custom_Transport : public Madara::Transport::Base
{
public:
  /**
    * Constructor
    * @param   id   unique identifer - usually a combination of host:port
    * @param   context  knowledge context
    * @param   config   transport configuration settings
    * @param   launch_transport  whether or not to launch this transport
    **/
  Custom_Transport (const std::string & id, 
    Madara::Knowledge_Engine::Thread_Safe_Context & context, 
    Madara::Transport::Settings & config, bool launch_transport);

  /**
    * Destructor
    **/
  ~Custom_Transport ();
      
  /**
    * Sends a list of knowledge updates to listeners
    * @param   updates listing of all updates that must be sent
    * @return  result of write operation or -1 if we are shutting down
    **/
  long send_data (const Madara::Knowledge_Records & updates);
	  
  /**
    * Closes the transport
    **/
  void close (void);
      
  /**
    * Accesses reliability setting
    * @return  whether we are using reliable dissemination or not
    **/
  int reliability (void) const;
      
  /**
    * Sets the reliability setting
    * @return  the changed setting
    **/
  int reliability (const int & setting);

  /**
    * Initializes the transport
    * @return  0 if success
    **/
  int setup (void);

private:
      
  /// host:port identifier of this process
  const std::string                         id_;

  /// thread for reading knowledge updates
  Custom_Transport_Read_Thread *            thread_;
      
  /// indicates whether the transport is correctly configured
  bool                                      valid_setup_;

  /// holds splitters for knowledge multiassignment expression for speed
  std::vector <std::string>                 splitters_;

  /// underlying socket for sending
  sockaddr_in                               socketAddress_;

  /// underlying socket for sending
  SOCKET                                    socket_;

  /// data received rules, defined in Transport settings
  //Madara::Expression_Tree::Expression_Tree  on_data_received_;

  /// buffer for sending
  Madara::Utility::Scoped_Array <char>      buffer_;
};

#endif // _CUSTOM_TRANSPORT_H_