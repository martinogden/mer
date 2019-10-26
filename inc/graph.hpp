#include <set>
#include <unordered_map>
#include <iostream>


typedef unsigned int Vertex;


// A simple graph
// adding self loops is an error
// adding multiple edges is ignored
class Graph {
private:
	unsigned int m;  // # edges
	unsigned int n;  // # vertices

	std::unordered_map<Vertex, std::set<Vertex>> adj;
	void prepare(Vertex& u, Vertex& v);
public:
	Graph();
	unsigned numEdges();
	unsigned numVertices();
	// add vertex if not added already
	void addVertex(Vertex v);
	bool hasVertex(Vertex v);
	// add edge if not added already
	// vertices u != v must exist
	void addEdge(Vertex u, Vertex v);
	bool hasEdge(Vertex u, Vertex v);

	std::set<Vertex> getVertices();
	std::set<Vertex> getNeighbors(Vertex v);
	friend std::ostream& operator<<(std::ostream& output, const Graph& G);
};
