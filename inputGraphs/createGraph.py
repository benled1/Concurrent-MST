import random
import csv

class Graph:
    def __init__(self, vertices):
        self.vertices = vertices
        self.edges = []

    def add_edge(self, v1, v2, weight):
        self.edges.append((v1, v2, weight))

    def generate_connected_graph(self):
        # Generate random edges to ensure connectivity
        for i in range(self.vertices - 1):
            self.add_edge(i, i + 1, random.randint(1, 100))

        # Connect the last vertex to the first one to close the loop
        self.add_edge(self.vertices - 1, 0, random.randint(1, 100))

    def write_to_csv(self, filename):
        with open(filename, 'w') as csvfile:
            writer = csv.writer(csvfile)
            for edge in self.edges:
                writer.writerow(edge)

if __name__ == "__main__":
    file_name = raw_input("Enter the filename for the graph: ")
    num_vertices = int(input("Enter the number of vertices: "))
    num_edges = int(input("Enter the number of edges (greater than or equal to vertices): "))

    if num_edges < num_vertices:
        print("Number of edges must be greater than or equal to the number of vertices.")
    else:
        graph = Graph(num_vertices)
        graph.generate_connected_graph()
        # Generate remaining random edges
        for _ in range(num_edges - num_vertices + 1):
            v1 = random.randint(0, num_vertices - 1)
            v2 = random.randint(0, num_vertices - 1)
            while v1 == v2:  # Ensure no self-loops
                v2 = random.randint(0, num_vertices - 1)
            weight = random.randint(1, 100)
            graph.add_edge(v1, v2, weight)

        graph.write_to_csv("inputGraphs/" + file_name + ".csv")
        print("CSV file created successfully.")

# Your isConnected method in C++

