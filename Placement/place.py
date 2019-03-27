import numpy as np
import sys


# Node class
class Node:
    def __init__(self, id=0, x=0, y=0):
        self.id = id
        self.x = x
        self.y = y
        self.edges = []

    def __str__(self):
        return str(self.id) + ":(" + str(self.x) + "," + str(self.y) + ")"

    def __repr__(self):
        print("<ID: " + str(self.id) + " -- Coord: (" + str(self.x) + ", " + str(self.y) +
              ")" + "\nEdges: " + str(self.edges))

    def add_edge(self, edge):
        if edge not in self.edges:
            self.edges.append(edge)
            self.edges.sort()


# Main function
def main():
    if len(sys.argv) < 2 or len(sys.argv) > 2:
        print("Incorrect number of arguments")
        sys.exit(1)

    # Parse input file
    file = open(sys.argv[1], 'r')  # Read first line
    line = file.readline()  # First line is move threshold
    move_thresh = float(line)  # Convert to float

    line = file.readline()  # Read second line
    line = line.split()
    num_fixed = int(line[0])  # Number of fixed points
    num_floating = int(line[1])  # Number of floating points
    num_edges = int(line[2])  # Number of edges in graph

    nodes = []

    # Read in locations of fixed points
    for i in range(num_fixed):
        line = file.readline()
        # Skip empty line, if there is one
        if len(line.strip()) == 0:
            line = file.readline()
        line = line.split()
        nodes.append(Node(i, int(line[0]), int(line[1])))

    # Add floating nodes
    for i in range(num_fixed, num_fixed + num_floating):
        nodes.append(Node(id=i))

    # Read in edges
    for i in range(num_edges):
        line = file.readline()
        # Skip empty line, if there is one
        if len(line.strip()) == 0:
            line = file.readline()
        line = line.split()
        # Add edges
        nodes[int(line[0])].add_edge(int(line[1]))
        nodes[int(line[1])].add_edge(int(line[0]))

    dim = num_floating   # Dimension of matrices is # of sets of coords being solved for

    # Solve x
    a = np.zeros((dim, dim))    # Matrix A
    b = np.zeros(dim)     # Matrix b
    # For each row of A and index of b
    for i in range(dim):
        count = 0
        # Sum x-coords of fixed points attached to the floating point
        j_range = len(nodes[i + num_fixed].edges)   # Iterate over each edge to the floating point
        for j in range(j_range):
            e = nodes[i + num_fixed].edges[j]   # Edge to check
            if e < num_fixed:   # Connected to fixed point
                b[i] = b[i] + abs(2 * nodes[e].x)  # Add 2*x of fixed point
        # Create row of A
        for j in range(dim):
            if i == j:
                a[i, j] = 2 * j_range   # Diagonal = 2 * # of edges the floating point is in
            elif (j + num_fixed) in nodes[i + num_fixed].edges:
                a[i, j] = -2    # If there's a connection to that floating point
                count += 1
            else:
                a[i, j] = 0     # No connections to other floating points

    x = np.linalg.solve(a, b)
    for i in range(dim):
        nodes[i + num_fixed].x = int(x[i])

    # Solve y
    # Matrix A should remain the same
    b = np.zeros(dim)     # Matrix b
    for i in range(dim):
        # Sum y-coordinates of fixed points attached to the floating point
        j_range = len(nodes[i + num_fixed].edges)   # Iterate over each edge to the floating point
        for j in range(j_range):
            e = nodes[i + num_fixed].edges[j]   # Edge to check
            if e < num_fixed:   # Connected to fixed point
                b[i] = b[i] + abs(2 * nodes[e].y)   # Add 2*y of fixed point
    y = np.linalg.solve(a, b)
    for i in range(dim):
        nodes[i + num_fixed].y = int(y[i])

    # Calculate Manhattan distances of edges
    total_distance = 0
    for n in nodes:
        for e in n.edges:
            # If edge id > node id, it hasn't been accounted for yet
            if e > n.id:
                x = abs(n.x - nodes[e].x)
                y = abs(n.y - nodes[e].y)
                total_distance += x + y
        if n.id >= num_fixed:       # TODO remove
            print(n.__repr__())
    print(total_distance)


if __name__ == '__main__':
    main()
