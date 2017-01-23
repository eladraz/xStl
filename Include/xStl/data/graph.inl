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

/*
 * graph.inl
 *
 * Implementation code of the cGraph class.
 *
 */
template <class T, class S>
cGraph<T,S>::cGraph(uint numberOfPossibleColors) :
    m_nodes(),
	m_numberOfPossibleColors(numberOfPossibleColors)
{
}

template <class T, class S>
cGraph<T,S>::~cGraph()
{
}

template <class T, class S>
cGraph<T,S>::cGraph(const cGraph<T,S> &other)
{
	m_nodes = other.getNodes();
}

template <class T, class S>
void cGraph<T,S>::addNode(const T& id, const S& value)
{
	/* Check if the node exists already (if so, don't do anything) */
	if (isNodeExists(id)) {
		return;
	}

	GraphNode<T,S> tmp(id, value, m_numberOfPossibleColors);	// The new node which will be entered.
	m_nodes.append(tmp);
}


template <class T, class S>
bool cGraph<T,S>::isConnected(const  T& id1, const T& id2)
{
	typename cList<GraphNode<T,S> >::iterator nodesIterator(m_nodes.begin());

	for (; nodesIterator != m_nodes.end(); ++nodesIterator)
	{
		GraphNode<T,S>& graphNode = *nodesIterator;
		if (id1 == graphNode.getId())
		{
			return graphNode.isNeighbor(id2);
		}
	}
	return false;
}

template <class T, class S>
void cGraph<T,S>::connectNodes(const T& id1, const T& id2, bool dup)
{
	typename cList<GraphNode<T,S> >::iterator current_node(m_nodes.begin());
	GraphNode<T,S>* graphNode1 = NULL;
	GraphNode<T,S>* graphNode2 = NULL;

	/* Find the graph nodes that correspond to the ids */
	for (; current_node != m_nodes.end(); ++current_node)
	{
		GraphNode<T,S>& graphNode = *current_node;
		if (graphNode.getId() == id1)
		{
			graphNode1 = &graphNode;
		}
		else if (graphNode.getId() == id2)
		{
			graphNode2 = &graphNode;
		}
	}

	/* If found, add the edge */
	if ((graphNode1 != NULL) && (graphNode2 != NULL))
	{
		if (dup)
		{
			graphNode1->addEdge(graphNode2);
			graphNode2->addEdge(graphNode1);
		}
		else
		{
			graphNode1->addEdgeNoDup(graphNode2);
			graphNode2->addEdgeNoDup(graphNode1);
		}
	}
}

template <class T, class S>
bool cGraph<T,S>::isEmpty() const
{
    return m_nodes.isEmpty();
}

template <class T, class S>
uint cGraph<T,S>::getNumberOfNodes() const
{
	return m_nodes.length();
}

template <class T, class S>
bool cGraph<T,S>::isNodeExists(const T& id) const
{
	typename cList<GraphNode<T,S> >::iterator nodesIterator(m_nodes.begin());
	for (; nodesIterator != m_nodes.end(); ++nodesIterator)
	{
		GraphNode<T,S>& graphNode = *nodesIterator;
		if (graphNode.getId() == id)
		{
			return true;
		}
	}
	return false;
}

template <class T, class S>
void cGraph<T,S>::makeClique(cList<T> ids)
{
	typename cList<T>::iterator idsIterator1(ids.begin());
	typename cList<T>::iterator idsIterator2(ids.begin());

	/* Add edges between every two nodes in the id list */
	for (; idsIterator1 != ids.end(); ++idsIterator1)
	{
		T& id1 = *idsIterator1;
		idsIterator2 = ids.begin();
		for (; idsIterator2 != ids.end(); ++idsIterator2)
		{
			T& id2 = *idsIterator2;
			connectNodes(id1, id2);
		}
	}
}

template <class T, class S>
GraphNode<T,S>* cGraph<T,S>::getNode(const T& id) const
{
	typename cList<GraphNode<T,S> >::iterator nodesIterator(m_nodes.begin());
	for (; nodesIterator != m_nodes.end(); ++nodesIterator)
	{
		GraphNode<T,S>& graphNode = *nodesIterator;
		if (graphNode.getId() == id)
		{
			return &(graphNode);
		}
	}
	return NULL;
}

template <class T, class S>
cList<GraphNode<T,S> >& cGraph<T,S>::getNodes()
{
	return m_nodes;
}

static bool isSetInOnePlace(cSetArray bitArray)
{
	bool found = false;

	for (uint i = 0 ; i < bitArray.getLength() ; ++i)
	{
		if (bitArray.isSet(i) && found)
			return false;
		else if (bitArray.isSet(i))
			found = true;
	}
	return found;
}
