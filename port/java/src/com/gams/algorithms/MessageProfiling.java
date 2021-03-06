/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 * 
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 * This material has been approved for public release and unlimited
 * distribution.
 * 
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *********************************************************************/
package com.gams.algorithms;

import java.util.HashSet;
import java.util.HashMap;

import com.gams.controllers.BaseController;
import com.gams.algorithms.AlgorithmStatusEnum;

import com.madara.KnowledgeBase;
import com.madara.transport.filters.Packet;
import com.madara.transport.TransportContext;
import com.madara.Variables;
import com.madara.logger.GlobalLogger;

/**
 * Base class that should be extended when creating a Java algorithm for
 * usage in the GAMS controller
 */
public class MessageProfiling extends BaseAlgorithm
{
  private static final String keyPrefix = "message_profiling";

  /**
   * @class MessageProfilingFilter
   * @brief The MessageProfilingFilter counts the expected messages received to 
   *        the KnowledgeBase
   **/
  private class MessageProfilingFilter implements com.madara.transport.filters.AggregateFilter
  {
    /**
     * MessageData struct
     */
    private class MessageData
    {
      public com.madara.containers.Integer first;
      public com.madara.containers.Integer last;
      public com.madara.containers.Double percentMissing;
      public HashSet<Long> missing;

      MessageData (String id, com.madara.Variables var)
      {
        String prefix = new String ("." + keyPrefix + "." + id + ".");

        first = new com.madara.containers.Integer ();
        com.madara.logger.GlobalLogger.log (6, "first.set_name");
        first.setName (var, prefix + "first");
        first.set (-1);
        
        last = new com.madara.containers.Integer ();
        com.madara.logger.GlobalLogger.log (6, "last.set_name");
        last.setName (var, prefix + "last");

        percentMissing = new com.madara.containers.Double ();
        com.madara.logger.GlobalLogger.log (6, "percent_missing.set_name");
        percentMissing.setName (var, prefix + "percent_missing");

        missing = new HashSet<Long> ();
      }
    };

    /**
     * Keep a MessageData struct for each peer
     */
    HashMap<String, MessageData> msgMap;

    /**
     * Filter the incoming packets
     */
    public void filter (Packet packet, TransportContext context, Variables variables)
    {
      // get/construct data struct
      String origin = context.getOriginator ();
      if (msgMap.get (origin) == null)
      {
        msgMap.put (origin, new MessageData(origin, variables));
      }
      MessageData data = msgMap.get (origin);
    
      // loop through each update
      for (String key : packet.getKeys ())
      {
        // we only care about specific messages
        com.madara.KnowledgeRecord record = packet.get (key);
        if (record.getType () == com.madara.KnowledgeType.STRING && 
          record.toString ().indexOf (keyPrefix) == 0)
        {
          // get msg number
          String[] split = record.toString ().split (",");
          long msgNum = Long.parseLong (split[0]);
    
          // is this the first?
          if (data.first.toLong () == -1)
          {
            data.first.set (msgNum);
            data.last.set (msgNum);
          }

          // is this a previously missing message?
          else if (msgNum < data.last.toLong ())
          {
            // is this earlier than first?
            if (msgNum < data.first.toLong ())
            {
              for (long i = msgNum + 1; i < data.first.toLong (); ++i)
                data.missing.add (i);
              data.first.set (msgNum);
            }
            else // it is between first and last, remove from missing set
            {
              data.missing.remove (msgNum);
            }
          }
          else // this could only be a new message past last
          {
            for (long i = data.last.toLong () + 1; i < msgNum; ++i)
              data.missing.add (i);
            data.last.set (msgNum);
          }
    
          long expected = data.first.toLong () - data.last.toLong () + 1;
          data.percentMissing.set ((double)data.missing.size () / (double) expected);
        }
      }
    }
  }

  /**
   * Filter object
   */
  private MessageProfilingFilter filter;

  /**
   * Message container
   */
  private com.madara.containers.String message;

  /**
   * Constructor
   **/
  public MessageProfiling ()
  {
    message = new com.madara.containers.String ();
    filter = new MessageProfilingFilter ();
  }

  /**
   * Initialize values, must be called after initAlgorithm for controller that will run algorithm
   * NOTE: DISABLED UNTIL KnowledgeBase.attachTransport (String, TransportSettings) is added to MADARA
   */
//  public void initVars (com.madara.transport.QoSTransportSettings settings)
//  {
//    // attach filter
//    settings.addReceiveFilter (filter);
//
//    // attach transport
//    knowledge.attachTransport (knowledge.getID (), settings);
//
//    // create message container
//    final String key = new String (keyPrefix + "." +
//      knowledge.get (".id").toString () + ".data");
//    message.setName (knowledge, key);
//  }

  /**
   * Analyzes the algorithm for new status information. This should be
   * a non-blocking call.
   * @return  status information (@see Status)
   **/
  public int analyze ()
  {
    return com.gams.algorithms.AlgorithmStatusEnum.OK.value();
  }
  
  /**
   * Plans next steps in the algorithm. This should be
   * a non-blocking call.
   * @return  status information (@see Status)
   **/
  public int plan ()
  {
    return com.gams.algorithms.AlgorithmStatusEnum.OK.value();
  }
  
  /**
   * Executes next step in the algorithm. This should be
   * a non-blocking call.
   * @return  status information (@see Status)
   **/
  public int execute ()
  {
    ++executions;

    // construct value
    String value = Long.toString (executions);
    value += ",aaaaaaaaaaaaaaaaaa";
    
    // actually set knowledge
    message.set (value);
  
    return com.gams.algorithms.AlgorithmStatusEnum.OK.value();
  }
}
