/*
 * Copyright (c) 2008-2016, Integrity Project Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Integrity Project nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 */

#ifndef __TBA_XSTL_DATA_MESSAGEQUEUE_H
#define __TBA_XSTL_DATA_MESSAGEQUEUE_H

/*
 * messageQueue.h
 *
 * The messageQueue is a component which holds messages for clients.
 * Each client has a unique ID in the system and has messages which link to
 * it. The queue manages the storing and polling of messages.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"


/*
 * Holds messages for clients. The program can access the queue, store messages
 * for one or more clients, poll the messages etc.
 * The queue has the following properties:
 *   - Template over the client-ID.
 *   - Template over group-ID
 *   - Saves the queue inside the repository. Can be used for presistance.
 *   - Stores a message for one or more clients.
 *   - Poll a message for a client.
 *   - Set a queue quota for a client (In memory bytes)
 *   - A watch-dog timmer allowing.
 *
 * The messages are stream of bytes.
 *
 * The client-id, which is templated in the name cClientID is a unique identifier
 * for the client. The template must implement the following operator:
 *   bool operator == (const cClientID& other) const;
 *   copy-constructor
 * This operator is in used in order to distinguish different clients.
 *
 * The client-id can be a number coressponding to the user, can be a process-ID
 * which seperates difference process which want to poll message from a shared
 * device. It can be even more complex struct which contain process-ID and a
 * client-ID for allowing multiple instance of device-objects inside a single
 * process...
 *
 * The group-ID has the following operator:
 *   bool isMine(const cClientID& id);
 * This function is in used to seprated clients to
 */
template <cClientID, cGroupID>
class cMessageQueue
{
public:
    /*
     * Constructor. Initialize empty queue.
     */
    cMessageQueue();

    /*
     * Destructor. Free all messages and queue memory.
     */
    ~cMessageQueue();

    /*
     * Stores a message for list of clients.
     *
     * clients - The clients ID list.
     * message - The message to be stored.
     *
     * Throws cMessageQueueException:
     *    ClientNotFound - One or more of the clients is not exist. The other
     *                     clients will contain the message in the queue.
     *    ClientQutaLimit - One or more of the clients has a quta-limit. Other
     *                      clients will contain the message in the queue.
     */
    void storeMessage(const cList<cClientID>& clientsID,
                      cBufferPtr message);

    /*
     * Polls a message for a client.
     *
     * clientID - The client ID
     *
     * Return a stream reference count for the message.
     *
     * Throws cMessageQueueException:
     *    ClientNotFound - The client cannot be found, or it's queue is empty.
     */
    cBufferPtr pollMessage(const cClientID& clientID);

    /*
     * Poll a message for a group.
     *
     * groupID - The group ID
     * clientID - Will be filled with the polled client identification
     *
     * Return a stream reference count for the message.
     */
    cBufferPtr pollMessage(const cGroupID& groupID,
                           cClientID* clientID);

private:
};


/*
 * MessageQueue which the group are irrelevant, use the cNullGroup as the
 * second argument for the cMessageQueue template argument.
 * The cNullGroup doesn't contains any clients.
 */
template <cClientID>
class cNullGroup
{
public:
    bool isMine(const cClientID& other)
    {
        return false;
    }
};

#endif // __TBA_XSTL_DATA_MESSAGEQUEUE_H

