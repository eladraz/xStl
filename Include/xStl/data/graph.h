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

/**
 * @addtogroup xstl_data_containers
 * @{
 */

#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/assert.h"
#include "xStl/data/list.h"
#include "xStl/data/setArray.h"

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to offer. Warning C4505 should be disabled for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif // XSTL_WINDOWS

/**
 * @brief A node of a cGraph.
 *
 * Each node holds a unique identifier, a value, the set of colors that the node
 * may be assigned and the list of its neighbors.
 *
 * @tparam T The type of the unique identifier of the node.
 * @tparam S The type of the value stored in the node.
 */
template <class T, class S>
class GraphNode
{
public:
    /* Constructor */
    /**
     * @brief Constructor. Create a node without a chosen color and without
     *    neighbors.
     *
     * @param id The unique identifier of the node.
     * @param value The value stored in the node.
     * @param numberOfPossibleColors The number of colors the node can be
     *    assigned.
     */
    GraphNode(const T& id, const S& value, uint numberOfPossibleColors) :
        m_id(id),
        m_value(value),
        m_possibleColors(numberOfPossibleColors, true, true),
        m_chosenColor(-1),
        m_neighbors(),
        m_numberOfPossibleColors(numberOfPossibleColors)
    {
    }

    /**
     * @brief Destructor
     */
    ~GraphNode()
    {
    }

    /**
     * @brief Returns the list of neighbors the node has
     *
     * @return The list of neighbors the node has.
     */
    cList<GraphNode*>& getNeighbors()
    {
        return m_neighbors;
    }

    /**
     * @brief Returns the (unique) ID of the node
     *
     * @return The (unique) ID of the node.
     */
    const T& getId() const
    {
        return m_id;
    }

    /**
     * @brief Returns the internal value in the node
     *
     * @return The internal value in the node.
     */
    S& getValue()
    {
        return m_value;
    }

    /**
     * @brief Set the internal value of the node.
     *
     * @param value The new value.
     */
    void setValue(S& value)
    {
        m_value = value;
    }

    /**
     * @brief Return the set of the possible colors of the node.
     *
     * @return The set of the possible colors of the node.
     */
    cSetArray& getPossibleColors()
    {
        return m_possibleColors;
    }

    /**
     * @brief Return the color chosen for the node.
     *
     * @return The chosen color.
     */
    int getChosenColor()
    {
        return m_chosenColor;
    }

    /**
     * @brief Set the color chosen for the node.
     *
     * @param color The chosen color. It must be lower than the number of
     *    possible colors.
     */
    void setChosenColor(uint color)
    {
        CHECK(color < m_numberOfPossibleColors);
        m_chosenColor = color;
    }

    /**
     * @brief Add the edge. Notice that this adds the edge even if it exists
     *
     * @param newNeighbor The node to add as a neighbor.
     */
    void addEdge(GraphNode* newNeighbor)
    {
        m_neighbors.append(newNeighbor);
    }

    /**
     * @brief Add the edge only if it did NOT exist before
     *
     * @param newNeighbor The node to add as a neighbor.
     */
    void addEdgeNoDup(GraphNode* newNeighbor)
    {
        /* Check if this new neighbor is already a neighbor */
        if (!isNeighbor(newNeighbor->getId()))
        {
            m_neighbors.append(newNeighbor);
        }
    }

    /**
     * @brief Check whether a node with the given identifier is a neighbor of
     *    this node.
     *
     * @param id The identifier to search for in the neighbors list.
     * @return True if a neighbor with the identifier @a id exists.
     */
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
    /**
     * @brief Unique identifier of the node
     */
    T m_id;
    /**
     * @brief Some data in the node
     */
    S m_value;
    /**
     * @brief Possible colors of node
     */
    cSetArray m_possibleColors;
    /**
     * @brief Chosen color for node.
     *
     * @details Initialized as 0.
     */
    uint m_chosenColor;
    /**
     * @brief List of neighbors this node has
     */
    cList<GraphNode*> m_neighbors;
    /**
     * @brief Number of possible colors
     */
    uint m_numberOfPossibleColors;
};


/**
 * @brief A graph of nodes which can be connected by edges and assigned colors.
 *
 * @details Contains standard implementation of graphs. This is a template graph
 *    for
 *   class T. T should have default constructor (due to using operator new[]).
 *
 * T must have:
 * default constructor.
 * For using connect, T must implement operator ==.
 *
 * @tparam T The type of the unique identifier of the nodes.
 * @tparam S The type of the value stored in the nodes.
 */
template <class T, class S>
class cGraph
{
public:

    /**
     * @brief Constructor - Create an empty graph
     *
     * @param numberOfPossibleColors The number of colors each node of the graph
     *    can be assigned.
     */
    cGraph(uint numberOfPossibleColors);

    /**
     * @brief Default destructor - Free all nodes in a graph
     */
    ~cGraph(void);

    /**
     * @brief Copy constructor. Creates a clone of the graph nodes.
     *
     * @param other The graph to clone.
     */
    cGraph(const cGraph<T,S> &other);

    /**
     * @brief Add a node to the graph.
     *
     * @param id The unique identifier of the new node.
     * @param value The value stored in the node.
     */
    void addNode(const T& id, const S& value);

    /**
     * @brief Check if the two nodes are connected.
     *
     * @details If the IDs don't represent nodes in the graph, then we return
     *    false.
     *
     * @param id1,id2 IDs of the two nodes.
     * @return A boolean: whether they are neighbors or not.
     */
    bool isConnected(const  T& id1, const T& id2);

    /**
     * @brief Connect two nodes by an edge.
     *
     * @details If an id is not in the graph, don't do anything.
     *
     * @param id1,id2 the two nodes.
     * @param dup By default an edge is not created if it is already present.
     */
    void connectNodes(const T& id1, const T& id2, bool dup = false);

    /**
     * @brief Checks if the graph has any nodes or not
     *
     * @return True if the graph has no nodes.
     */
    bool isEmpty() const;

    /**
     * @brief Return the number of nodes in the graph
     *
     * @return The number of nodes in the graph.
     */
    uint getNumberOfNodes() const;

    /**
     * @brief Checks if a node exists in the graph
     *
     * @param id the id of the node to search.
     * @return True if a node with the identifier @a id exists in the graph.
     */
    bool isNodeExists(const T& id) const;

    /**
     * @brief Connect every pair of the given nodes.
     *
     * The function assumes that the graph already contains all the nodes.
     *
     * @param ids The identifiers of the nodes that form the clique.
     */
    void makeClique(cList<T> ids);

    /**
     * @brief Return the data (value) inside a node indexed by the unique "id".
     *
     * @return The data (value) inside a node indexed by the unique "id".
     * @param id The unique identifier of the node.
     */
    GraphNode<T,S>* getNode(const T& id) const;

    /**
     * @brief Return the list of nodes in the graph.
     *
     * @return The list of nodes in the graph.
     */
    cList<GraphNode<T,S> >& getNodes();

private:
    /**
     * @brief List of nodes in the graph
     */
    cList<GraphNode<T,S> > m_nodes;

    /**
     * @brief Number of possible colors
     */
    uint m_numberOfPossibleColors;
};

// Implement the template function.
#include "xStl/data/graph.inl"

#ifdef XSTL_WINDOWS
    // Return the warning behaviour back to its original mode
    #pragma warning(pop)
#endif


/** @} */
#endif // __TBA_STL_GRAPH_H
