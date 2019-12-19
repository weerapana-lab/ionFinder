
import argparse
import re
import sys

def read_source(fname):
    """read_source

    :param fname: Path to variable source file.

    :return: dict of variables.
    """

    lines = list()
    with open(fname, 'r') as inF:
        for line in inF:
            l = line.strip()
            l = l.partition('#')[0]
            if l:
                lines.append(l)

    ret = dict()
    for line in lines:
        m = re.search(r'^(\w+)=([\'\"](.+)[\"\']|.+)$', line)
        if not m:
            raise SyntaxError("Could not parse line: {}".format(line))
        ret[m.group(1)] = m.group(2) if m.group(3) is None else m.group(3)
    
    return ret


def substutute_variabies(variables, template_path, output_path,
                         sub_re = r'@(\w+)@'):
    with open(template_path, 'r') as inF:
        template_lines = inF.readlines()
    
    output_lines = list()
    for line in template_lines:
        m = re.search(sub_re, line)
        if m and m.group(1) in variables:
            repl = variables[m.group(1)]
            line = re.sub(sub_re, repl, line)
        output_lines.append(line)
            
    with open(output_path, 'w') as outF:
        for line in output_lines:
            outF.write(line)


def main():
    parser = argparse.ArgumentParser(prog='configure_file',
            description='Substutute variables in config file a la cmake configure_file function.')
    
    parser.add_argument('source', help='Text file with variable definitions.')
    parser.add_argument('input', help='Template file with variable references to substutute')
    parser.add_argument('output', help='Destination file.')

    args = parser.parse_args()
    
    sys.stdout.write('Reading variables from {}...'.format(args.source))
    variables = read_source(args.source)
    sys.stdout.write('\tDone!\n')
    substutute_variabies(variables, args.input, args.output)
    sys.stdout.write('Configured file written to: {}\n'.format(args.output))


if __name__ == "__main__":
    main()


