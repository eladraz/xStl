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

#ifndef __TBA_STL_GRAPH_H
#define __TBA_STL_GRAPH_H

#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/assert.h"
#include "xStl/data/list.h"
#include "xStl/data/setArray.h"

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to ofer. Warning C4505 should be over-written for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif // XSTL_WINDOWS

template <class T, class S>
class GraphNode
{
public:
    /* Construcotr */
    GraphNode(const T& id, const S& value, uint numberOfPossibleColors) :
        m_id(id),
        m_value(value),
        m_possibleColors(numberOfPossibleColors, true, true),
        m_chosenColor(-1),
        m_neighbors(),
        m_numberOfPossibleColors(numberOfPossibleColors)
    {
    }

    /* Destructor */
    ~GraphNode()
    {
    }

    /* Returns the list of neighbors the node has */
    cList<GraphNode*>& getNeighbors()
    {
        return m_neighbors;
    }

    /* Returns the (uniquee) ID of the node */
    const T& getId() const
    {
        return m_id;
    }

    /* Returns the internal value in the node */
    S& getValue()
    {
        return m_value;
    }

    void setValue(S& value)
    {
        m_value = value;
    }

    cSetArray& getPossibleColors()
    {
        return m_possibleColors;
    }

    int getChosenColor()
    {
        return m_chosenColor;
    }

    void setChosenColor(uint color)
    {
        CHECK(color < m_numberOfPossibleColors);
        m_chosenColor = color;
    }

    /* Add the edge. Notice that this adds the edge even if it exists */
    void addEdge(GraphNode* newNeighbor)
    {
        m_neighbors.append(newNeighbor);
    }

    /* Add the edge only if it did NOT exist before */
    void addEdgeNoDup(GraphNode* newNeighbor)
    {
        /* Check if this new neighbor is already a neighbor */
        if (!isNeighbor(newNeighbor->getId()))
        {
            m_neighbors.append(newNeighbor);
        }
    }

    bool isNeighbor(const T& id) const
    {
        typename cList<GraphNode*>::iterator current_node(m_neighbors.begin());
        for (; current_node != m_neighbors.end(); ++current_node)
        {
            GraphNode* graphNode = *current_node;
            if (graphNode->getId() == id)
            {
                return true;
            }
        }
        return false;
    }

private:
    // Unique identifier of the node
    T m_id;
    // Some data in the node
    S m_value;
    // Possible colors of node
    cSetArray m_possibleColors;
    // Chosen color for node. Initialized as 0.
    uint m_chosenColor;
    // List of neighbors this node has
    cList<GraphNode*> m_neighbors;
    // Number of possible colors
    uint m_numberOfPossibleColors;
};


/*
 * cGraph template class.
 *
 * Contains standard implementation of graphs. This is a template graph for
 * class T. T should have default constructor (due to using operator new[]).
 *
 * T must have:
 *    default constructor.
 *    For using connect, T must implement operator ==.
 *
 */
template <class T, class S>
class cGraph
{
public:

    /* Constructor - Create an empty graph */
    cGraph(uint numberOfPossibleColors);

    /* Default destructor - Free all nodes in a graph */
    ~cGraph(void);

    /*
     * Copy constructor. Creates a clone of the graph nodes.
     */
    cGraph(const cGraph<T,S> &other);

    /*
     * Add a node to the graph
     *
     * node - The object to be added.
     */
    void addNode(const T& id, const S& value);

    /* Check if the two nodes are connected.
     * If the IDs dont represent nodes in the graph, then we return false.
     * id1, id2 - IDs of the two nodes.
     * Returns - boolean if they are neighbors or not.
     */
    bool isConnected(const  T& id1, const T& id2);

    /*
     * Connect two nodes by an edge. If an id is not in the graph, don't do anything.
     * id1, id2 - the two nodes.
     * dup - By default an edge is not created if it is already present.
     */
    void connectNodes(const T& id1, const T& id2, bool dup = false);

    /*
     * Checks if the graph has any nodes or not
     */
    bool isEmpty() const;

    /*
     * Return the number of nodes in the graph
     */
    uint getNumberOfNodes() const;

    /*
     * Checks if a node exists in the graph
     * id - the id of the node to search.
     */
    bool isNodeExists(const T& id) const;

    /*
     * This function assumes that the graph already contains all the nodes
     */
    void makeClique(cList<T> ids);

    /*
     * Return the data (value) inside a node indexed by the unique "id".
     */
    GraphNode<T,S>* getNode(const T& id) const;

    /*
     * Return the list of nodes in the graph.
     */
    cList<GraphNode<T,S> >& getNodes();

private:
    // List of nodes in the graph
    cList<GraphNode<T,S> > m_nodes;

    // Number of possible colors
    uint m_numberOfPossibleColors;
};

// Implement the template function.
#include "xStl/data/graph.inl"

#ifdef XSTL_WINDOWS
    // Return the warnning behaviour back to it's original mode
    #pragma warning(pop)
#endif

#endif // __TBA_STL_GRAPH_H
