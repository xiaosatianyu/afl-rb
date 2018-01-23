#!/usr/bin/env python3
import argparse
import networkx as nx
import re

#Regular expression to find callee
pattern = re.compile('@.*?\(')

def node_name (name):
    return "\"{%s}\"" % name

#################################
# Find the graph node for a name
#################################
def find_nodes (name):
  n_name = node_name (name)
  n_list = list (filter (lambda d: 'label' in d[1] and n_name in d[1]['label'], G.nodes(data=True)))
  if len (n_list) > 0:
    return n_list
  else:
    return []

##################################
# Calculate Distance
##################################
def distance (name):
  distance = -1
  for (n, _) in find_nodes (name):
    d = 0.0
    i = 0
    if is_cg:
      for (t, _) in targets:
        if nx.has_path (G, n, t):
          shortest = nx.dijkstra_path_length (G, n, t)
          d += 1.0 / (1.0 + shortest)
          i += 1
    else:
      for t_name in bb_distance:
        di = 0.0
        ii = 0
        for (t, _) in find_nodes(t_name):
          #Check if path exists
          if nx.has_path (G, n, t) :
            shortest = nx.dijkstra_path_length(G, n, t)
            di += 1.0 / (1.0 + 10 * bb_distance[t_name] + shortest)
            ii += 1
        if ii != 0:
          d += di / ii
          i += 1

    if d != 0 and (distance == -1 or distance > i / d) :
      distance = i / d

  if distance != -1:
    out.write (name)
    out.write (",")
    out.write (str (distance))
    out.write ("\n")

##加载目标函数
def load_target(targets_file):
    print ("Loading targets..")
    with open(targets_file, "r") as f:
        targets = []
        for line in f.readlines ():
            line = line.strip ()
            for target in find_nodes(line):
                targets.append (target)
    
    if (len (targets) == 0):
        print ("No targets available")
        exit(1)

    return targets

#计算每个函数的距离
def cal_dis(out_file, names_file):
    print ("Calculating distance..")
    with open(out_file, "w") as out:
        with open(names_file, "r") as f:
            for line in f.readlines():
                line = line.strip()
                distance (line)


# Main function
if __name__ == '__main__':
  parser = argparse.ArgumentParser ()
  parser.add_argument ('-d', '--dot', type=str, required=True, help="Path to dot-file representing the graph.")
  parser.add_argument ('-t', '--targets', type=str, required=True, help="Path to file specifying Target nodes.")
  parser.add_argument ('-o', '--out', type=str, required=True, help="Path to output file containing distance for each node.")
  parser.add_argument ('-n', '--names', type=str, required=True, help="Path to file containing name for each node.")
  parser.add_argument ('-c', '--cg_distance', type=str, help="Path to file containing call graph distance.")
  parser.add_argument ('-s', '--cg_callsites', type=str, help="Path to file containing mapping between basic blocks and called functions.")

  args = parser.parse_args ()

  print ("\nParsing %s .." % args.dot)
  G = nx.DiGraph(nx.drawing.nx_pydot.read_dot(args.dot))
  print (nx.info(G))

  is_cg = 1 if "Name: Call graph" in nx.info(G) else 0
  print ("\nWorking in %s mode.." % ("CG" if is_cg else "CFG"))

  #1. load target
  targets = load_target(args.targets)
  #2. calculate distance
  cal_dis(args.out, args.name)
 
