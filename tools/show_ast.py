import graphviz
import sys
import json
import random

inside_set = False


#shape_config = {'shape':'none'}
shape_config = {'shape':'box'}


def build_graph(g, node):
    global inside_set

    node_name = str(id(node))
    label = node['type']

    if node['type'] == 'char_set':
        if node['negative'] == True:
            label = f"^{label}"

    g.node(node_name, label, **shape_config)

    if node['type'] in ['alternation', 'concat']:
        if 'left' in node: g.edge(node_name, build_graph(g, node['left']))
        if 'right' in node: g.edge(node_name, build_graph(g, node['right']))

    elif node['type'] == 'str':
        lit_name = node_name + ".str"
        s = node['str']
        label = f'"{s}"'
        g.node(lit_name, label, **shape_config)
        g.edge(node_name, lit_name)

    elif node['type'] == 'char':
        lit_name = node_name + ".char"
        ch = node['char']
        label = f"'{chr(ch)}'"
        g.node(lit_name, label, **shape_config)
        g.edge(node_name, lit_name)

    elif node['type'] == 'group':
        expr = build_graph(g, node['expr'])
        g.edge(node_name, expr)

    elif node['type'] == 'start_of_string':
        pass

    elif node['type'] == 'char_class':
        class_name = node_name + ".class"
        label = node['class']
        g.node(class_name, label, **shape_config)
        g.edge(node_name, class_name)

    elif node['type'] == 'char_set':
        inside_set = True
        items_name = node_name + ".items"
        g.node(items_name, 'items', **shape_config)
        g.edge(node_name, items_name)

        for item in node['items']:
            item_node = build_graph(g, item)
            g.edge(items_name, item_node)
        inside_set = False

    elif node['type'] == 'char_range':
        range_name = node_name + ".range"
        lower_name = range_name + '.l'
        upper_name = range_name + '.u'
        lower = chr(node['range'][0])
        upper = chr(node['range'][1])
        lower = f"'{lower}'"
        upper = f"'{upper}'"
        g.node(lower_name, lower, **shape_config)
        g.node(upper_name, upper, **shape_config)
        g.edge(node_name, lower_name, 'L')
        g.edge(node_name, upper_name, 'R')

    elif node['type'] == 'quantifier':
        lower = str(node['range'][0])
        if node['range'][1] == 0xffffffff:
            upper = 'inf'
        else:
            upper = str(node['range'][1])

        lower_name = node_name + '.l'
        upper_name = node_name + '.u'
        g.node(lower_name, lower, **shape_config)
        g.node(upper_name, upper, **shape_config)
        g.edge(node_name, lower_name, 'L')
        g.edge(node_name, upper_name, 'R')


    if 'quantifier' in node and not inside_set:
        quantifier = build_graph(g, node['quantifier'])
        g.edge(node_name, quantifier)

    return node_name


def main(filename):
    print(filename)

    data = {}
    with open(filename) as f:
        data = json.load(f)

    #print(data.keys())
    g = graphviz.Digraph(format='png')

    regex = data['regex']
    g.node('regex', f'RE: {regex}', shape='none', pos='-1,1!', width='0', height='0')

    build_graph(g, data['ast'])
    g.render('/tmp/ast', view=True)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"usage: {sys.argv[0]} ast.json")
        sys.exit(1)
    main(sys.argv[1])
