#pragma once
#include <iostream>
#include <unordered_map>
#include <unordered_set>


template <typename T>
class DiGraph {
protected:
	unsigned n;
	std::unordered_map<T, std::unordered_set<T>> adj;

public:
	DiGraph() : n(0) {}
	virtual ~DiGraph() = default;

	unsigned numVertices() {
		return n;
	}

	void addVertex(T v) {
		if ( hasVertex(v) )
			return;

		adj[v] = {};
		n++;
	}

	bool hasVertex(T v) {
		return adj.find(v) != adj.end();
	}

	void removeVertex(T v) {
		if ( !hasVertex(v) )
			return;

		std::unordered_set<T> V = adj[v];
		for (const auto& u : V)
			removeEdge(u, v);
		adj.erase(v);
		n--;
	}

	virtual void addEdge(T u, T v) {
		assert( hasVertex(u) && hasVertex(v) );
		assert(u != v);
		adj[u].insert(v);
	}

	bool hasEdge(T u, T v) {
		if ( !hasVertex(u) || !hasVertex(v) )
			return false;
		return adj[u].find(v) != adj[u].end();
	}

	virtual void removeEdge(T u, T v) {
		if ( !hasVertex(u) || !hasVertex(v) )
			return;

		adj[u].erase(v);
	}

	std::unordered_set<T> getAdj(T v) {
		assert(hasVertex(v));
		return adj[v];
	}

	std::unordered_set<T> getVertices() {
		std::unordered_set<T> V;
		for (const auto& pair : adj)
			V.insert(pair.first);
		return V;
	}

	friend std::ostream& operator<<(std::ostream& output, const DiGraph& G);
};


// A simple graph
// adding self loops is an error
// adding multiple edges is ignored
template <typename T>
class Graph : public DiGraph<T> {
public:
	void addEdge(T u, T v) override {
		DiGraph<T>::addEdge(u, v);
		DiGraph<T>::addEdge(v, u);
	}

	void removeEdge(T u, T v) override {
		DiGraph<T>::removeEdge(u, v);
		DiGraph<T>::removeEdge(v, u);
	}

	friend std::ostream& operator<<(std::ostream& output, const Graph& G) {
		output << "graph G {" << std::endl;
		std::unordered_set<std::string> added;
		for (auto& adj : G.adj)
			output << "\t\"" << adj.first.to_string() << "\"" << std::endl;

		for (auto& adj : G.adj) {
			T u = adj.first;

			for (T v : adj.second) {
				if (added.find(u.to_string() + v.to_string()) == added.end()) {
					output << "\t\"" << u.to_string() << "\" -- \"" << v.to_string() << "\"" << std::endl;
					added.insert(u.to_string() + v.to_string());
					added.insert(v.to_string() + u.to_string());
				}
			}
		}

		output << "}";

		return output;
	}
};