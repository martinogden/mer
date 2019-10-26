#include "graph.hpp"


void swap(Vertex& u, Vertex& v) {
	Vertex t = u;
	u = v;
	v = t;
}


Graph::Graph() :
	m(0), n(0)
{}


unsigned Graph::numEdges() {
	return m;
}


unsigned Graph::numVertices() {
	return n;
}


void Graph::prepare(Vertex& u, Vertex& v) {
	assert(hasVertex(u) && hasVertex(v) && "vertex doesn't exist");
	assert(u != v && "simple graph has no self-loops");
	if (u > v)
		swap(u, v);
}


void Graph::addVertex(Vertex v) {
	if (!hasVertex(v)) {
		adj[v] = {};
		n++;
	}
}


bool Graph::hasVertex(Vertex v) {
	return adj.find(v) != adj.end();
}


void Graph::addEdge(Vertex u, Vertex v) {
	prepare(u, v);
	if (!hasEdge(u, v)) {
		adj[u].insert(v);
		adj[v].insert(u);
		m++;
	}
}


bool Graph::hasEdge(Vertex u, Vertex v) {
	prepare(u, v);
	return adj[u].find(v) != adj[u].end();
}


std::set<Vertex> Graph::getVertices() {
	std::set<Vertex> V;

	for (auto& item : adj)
		V.insert(item.first);

	return V;
}


std::set<Vertex> Graph::getNeighbors(Vertex v) {
	assert(hasVertex(v) && "vertex doesn't exist");

	return adj[v];
}
