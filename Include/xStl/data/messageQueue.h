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

/**
 * @addtogroup xstl_data_containers
 * @{
 */

/**
 * @file messageQueue.h
 * @brief The messageQueue is a component which holds messages for clients.
 *
 * @details Each client has a unique ID in the system and has messages which
 *    link to it. The queue manages the storing and polling of messages.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"


/**
 * @brief Holds messages for clients.
 *
 * @details The program can access the queue, store messages for one or more
 *    clients, poll the messages etc. The queue has the following properties: -
 *    Template over the client-ID. - Template over group-ID - Saves the queue
 *    inside the repository. Can be used for persistence. - Stores a message for
 *    one or more clients. - Poll a message for a client. - Set a queue quota
 *    for a client (In memory bytes) - A watch-dog timer is supported.
 *
 * The messages are stream of bytes.
 *
 * The client-id, which is templated in the name cClientID is a unique identifier
 * for the client. The template must implement the following operator:
 * bool operator == (const cClientID\& other) const;
 * copy-constructor
 * This operator is used to distinguish between different clients.
 *
 * The client-id can be a number corresponding to the user, can be a process-ID
 * which separates different processes which want to poll messages from a shared
 * device. It can be an even more complex struct which contains a process-ID and
 * a
 * client-ID, allowing multiple instances of device-objects inside a single
 * process...
 *
 * The group-ID has the following operator:
 * bool isMine(const cClientID\& id);
 * This function is used to separate clients into groups.
 *
 * @tparam cClientID The type of the client identifier.
 * @tparam cGroupID The type of the group identifier. It must provide `bool
 *    isMine(const cClientID&)`.
 */
template <cClientID, cGroupID>
class cMessageQueue
{
public:
    /**
     * @brief Constructor. Initialize empty queue.
     */
    cMessageQueue();

    /**
     * @brief Destructor. Free all messages and queue memory.
     */
    ~cMessageQueue();

    /**
     * @brief Stores a message for list of clients.
     *
     * @details clients - The clients ID list.
     *
     * Throws cMessageQueueException: ClientNotFound - One or more of the
     * clients do not exist. The other clients will contain the message in the
     * queue. ClientQuotaLimit - One or more of the clients has a quota-limit.
     * Other clients will contain the message in the queue.
     *
     * @param message The message to be stored.
     * @param clientsID The identifiers of the clients which receive the
     *    message.
     */
    void storeMessage(const cList<cClientID>& clientsID,
                      cBufferPtr message);

    /**
     * @brief Polls a message for a client.
     *
     * @details Throws cMessageQueueException: ClientNotFound - The client
     *    cannot be found, or its queue is empty.
     *
     * @param clientID The client ID
     * @return A stream reference count for the message.
     */
    cBufferPtr pollMessage(const cClientID& clientID);

    /**
     * @brief Poll a message for a group.
     *
     * @param groupID The group ID
     * @param clientID Will be filled with the polled client identification
     * @return A stream reference count for the message.
     */
    cBufferPtr pollMessage(const cGroupID& groupID,
                           cClientID* clientID);

private:
};


/**
 * @brief MessageQueue for which the group is irrelevant, use the cNullGroup as
 *    the
 *   second argument for the cMessageQueue template argument.
 *
 * @details The cNullGroup doesn't contain any clients.
 *
 * @tparam cClientID The type of the client identifier.
 */
template <cClientID>
class cNullGroup
{
public:
    /**
     * @brief Check whether a client belongs to the group.
     *
     * @param other The client identifier.
     * @return Always false. The group contains no clients.
     */
    bool isMine(const cClientID& other)
    {
        return false;
    }
};


/** @} */
#endif // __TBA_XSTL_DATA_MESSAGEQUEUE_H

