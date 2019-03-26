import numpy as np
import sys


# Node class
class Node:
    def __init__(self, id=0, x=0.0, y=0.0):
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
        self.edges.append(edge)


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
        nodes.append(Node(i, float(line[0]), float(line[1])))

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


if __name__ == '__main__':
    main()
