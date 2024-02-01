import argparse
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['font.family'] = 'Times New Roman'

parser = argparse.ArgumentParser(description='Read data from file and print a PrettyTable.')
parser.add_argument('input_file_path', type=str, help='Path to the input file')

args = parser.parse_args()

xlabel = 'Arrival Order'

ylabel = 'Departure Order'

# flow_color_dict = {0: '#4682A9', 1: '#749BC2', 2: '#91C8E4', 3: '#FF6000', 4: '#FFA559', 5: '#FFE6C7'}
flow_color_dict = {0: '#11009E', 1: '#2D31FA', 2: '#5D8BF4', 3: '#B70404', 4: '#F94C10', 5: '#FF9B50'}

def get_pfmap_file_name(input_file_name):
    dot_pos = input_file_name.find(".output")

    assert dot_pos != -1, ".output not found in input file name"

    output_file_name = input_file_name[:dot_pos] + ".pfmap" + input_file_name[dot_pos + 7:]

    return output_file_name

def get_refoutput_file_name(input_file_name):
    dot_pos = input_file_name.find(".output")

    assert dot_pos != -1, ".output not found in input file name"

    output_file_name = input_file_name[:dot_pos] + ".ref" + input_file_name[dot_pos + 7:]

    return output_file_name

def get_fig_file_name(input_file_name):
    dot_pos = input_file_name.find(".output")

    assert dot_pos != -1, ".output not found in input file name"

    output_file_name = input_file_name[:dot_pos] + ".png" + input_file_name[dot_pos + 7:]

    return output_file_name

def get_reffig_file_name(input_file_name):
    dot_pos = input_file_name.find(".output")

    assert dot_pos != -1, ".output not found in input file name"

    output_file_name = input_file_name[:dot_pos] + ".ref.png" + input_file_name[dot_pos + 7:]

    return output_file_name

def get_rawdata_file_name(input_file_name):
    dot_pos = input_file_name.find(".output")

    assert dot_pos != -1, ".output not found in input file name"

    output_file_name = input_file_name[:dot_pos] + ".rawdata.txt" + input_file_name[dot_pos + 7:]

    return output_file_name

pfmap_file_path = get_pfmap_file_name(args.input_file_path)
output_file_path = args.input_file_path+'.txt'
refoutput_file_path = get_refoutput_file_name(args.input_file_path)
fig_file_path = get_fig_file_name(args.input_file_path)
reffig_file_path = get_reffig_file_name(args.input_file_path)
rawdata_file_path = get_rawdata_file_name(args.input_file_path)


with open(pfmap_file_path, 'r') as file:
    lines = file.readlines()

pfmap = {}

# get pfmap
for line in lines:

    parts = line.strip().split(', ')
    
    meta_value = int(parts[0].split(':')[1])
    flow_value = int(parts[1].split(':')[1])
    
    pfmap[meta_value] = flow_value


def draw_fig(output_file_path, fig_file_path, rawdata_file_path):

    plt.clf()
    plt.figure()

    diff_cyc = {}
    push_cyc = {}

    with open(output_file_path, 'r') as file:
        lines = file.readlines()

    for line in lines:
        parts = line.strip().split(', ')
        
        meta_value = int(parts[0].split(':')[1])
        cyc_type, cyc_value = parts[1].split(':')
        cyc_value = int(cyc_value)

        if cyc_type == 'push_cyc':
            diff_cyc[meta_value] = cyc_value
            push_cyc[meta_value] = cyc_value
        elif cyc_type == 'pop_cyc':
            diff_cyc[meta_value] = cyc_value
            # diff_cyc[meta_value] = cyc_value - diff_cyc[meta_value]
        else:
            assert(0)

    flow_diff_cyc_lists = {}

    for meta_value, diff_value in diff_cyc.items():
        flow_value = pfmap.get(meta_value)
        if flow_value not in flow_diff_cyc_lists:
            flow_diff_cyc_lists[flow_value] = []
        flow_diff_cyc_lists[flow_value].append(diff_value)

    flow_push_cyc_lists = {}

    for meta_value, push_value in push_cyc.items():
        flow_value = pfmap.get(meta_value)
        if flow_value not in flow_push_cyc_lists:
            flow_push_cyc_lists[flow_value] = []
        flow_push_cyc_lists[flow_value].append(push_value)

    for flow_value in flow_diff_cyc_lists.keys():
        flow_diff_cyc_list = flow_diff_cyc_lists.get(flow_value)
        flow_push_cyc_list = flow_push_cyc_lists.get(flow_value)
        
        if flow_push_cyc_list is not None and flow_diff_cyc_list is not None:
            plt.scatter(flow_push_cyc_list, flow_diff_cyc_list, s=0.4,label=f'Flow {flow_value}')

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)

    plt.legend()

    plt.tight_layout()

    plt.savefig(fig_file_path, dpi=1200)

    plt.clf()
    plt.figure()
    plt.tick_params(axis='both', which='major', labelsize=16)

    start_index = 0
    sorted_keys = sorted(flow_diff_cyc_lists.keys())
    with open(rawdata_file_path, 'w') as f:
        for flow_value in sorted_keys:
            flow_diff_cyc_list = flow_diff_cyc_lists.get(flow_value)
            flow_push_cyc_list = flow_push_cyc_lists.get(flow_value)

            # flow_diff_cyc_list = flow_diff_cyc_list[start_index : start_index + 10]
            # flow_push_cyc_list = flow_push_cyc_list[start_index : start_index + 10]
            
            if flow_push_cyc_list is not None and flow_diff_cyc_list is not None:
                if(flow_value < 3):
                    plt.scatter(flow_push_cyc_list, flow_diff_cyc_list, color=flow_color_dict[flow_value], s=12, marker='^', label=f'Tenant A Flow {flow_value}')
                    print(f'Tenant A Flow {flow_value}', file=f)
                    
                else:
                    plt.scatter(flow_push_cyc_list, flow_diff_cyc_list, color=flow_color_dict[flow_value], s=12, marker='s', label=f'Tenant B Flow {flow_value}')
                    print(f'Tenant B Flow {flow_value}', file=f)

                print(xlabel, file=f)
                print(flow_push_cyc_list, file=f)
                print(ylabel, file=f)
                print(flow_diff_cyc_list, file=f)
                print('', file=f)

    # plt.gca().plot(plt.gca().get_xlim(), plt.gca().get_ylim(), ls='--', c='black', linewidth=0.8)

    plt.xlabel(xlabel, fontsize=16)
    plt.ylabel(ylabel, fontsize=16)

    
    legend = plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc='lower left',
           ncol=3, fontsize=10, mode="expand", borderaxespad=0.)

    if(fig_file_path == '../compiler_test/test2_2.ref.png'):
        plt.annotate('inflection point', xy=(62, 100), xytext=(65, 110),
             arrowprops=dict(arrowstyle='->', linewidth=0.8), fontsize=8)


    plt.tight_layout()
    plt.savefig(fig_file_path+'.start.png', dpi=500)




# draw_fig(output_file_path, fig_file_path)
draw_fig(refoutput_file_path, reffig_file_path, rawdata_file_path)


