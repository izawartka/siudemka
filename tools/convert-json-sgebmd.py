import struct
import json
import argparse
import os

_format_version_ = 6
_float_scale_ = 20000
global_tilemap_indexes = {}
global_tileset_indexes = {}

def get_tileset_indexes(tilemaps_json):
    return {tilemap['name']: i for i, tilemap in enumerate(tilemaps_json)}


def get_input_indexes(submodels_json):
    input_indexes = {
        'rot_base': 0
    }

    for submodel in submodels_json.values():
        input_names = [submodel.get('index_by', ''), submodel.get('condition', ''), submodel.get('rot_by', '')]
        for input_name in input_names:
            if not input_name:
                continue
            if input_name not in input_indexes:
                input_indexes[input_name] = len(input_indexes)

    return input_indexes


def get_submodel_indexes(submodels_json):
    return {submodel_name: i for i, submodel_name in enumerate(submodels_json.keys())}


def write_info_block(file, info_json):
    model_name = info_json['name'].encode('utf-8')
    model_name_length = len(model_name)

    file.write(b'INFO')
    block_size = 3 + model_name_length + 8
    file.write(struct.pack('I', block_size))
    file.write(struct.pack('H', _format_version_))
    file.write(struct.pack('B', model_name_length))
    file.write(model_name)
    file.write(struct.pack('I', int(info_json.get('originX', 0) * _float_scale_)))
    file.write(struct.pack('I', int(info_json.get('originY', 0) * _float_scale_)))


def write_inputs_block(file, input_indexes):
    inputs_data = b''
    for input_name, input_index in input_indexes.items():
        name = input_name.encode('utf-8')
        inputs_data += struct.pack('B', len(name)) + name

    file.write(b'INPT')
    block_size = 2 + len(inputs_data)
    file.write(struct.pack('I', block_size))
    file.write(struct.pack('H', len(input_indexes)))
    file.write(inputs_data)


# also saves tilemap indexes to a global variable
def write_tilemaps_block(file, tilemaps_json):
    global global_tilemap_indexes

    tilemaps_data = b''
    for tilemap_index, tilemap_json in enumerate(tilemaps_json):
        tilemaps_data += get_tilemap_def_data(tilemap_json)
        tilemap_name = tilemap_json['name']
        global_tilemap_indexes[tilemap_name] = tilemap_index

    file.write(b'TATL')
    block_size = 2 + len(tilemaps_data)
    file.write(struct.pack('I', block_size))
    file.write(struct.pack('H', len(tilemaps_json)))
    file.write(tilemaps_data)


def get_tilemap_def_data(tilemap_json):
    base_image = tilemap_json['baseImage'].encode('utf-8')
    tilemap_data = struct.pack('B', len(base_image)) + base_image

    return tilemap_data


def write_tilesets_block(file, tilemaps_json, assets_dir):
    all_tilesets = []
    for tilemap_json in tilemaps_json:
        all_tilesets += get_tilesets(tilemap_json, assets_dir)

    tilesets_data = get_tilesets_data(all_tilesets)

    file.write(b'TSET')
    block_size = 2 + len(tilesets_data)
    file.write(struct.pack('I', block_size))
    file.write(struct.pack('H', len(all_tilesets)))
    file.write(tilesets_data)


# also saves tileset indexes to a global variable
def get_tilesets(tilemap_json, assets_dir):
    global global_tileset_indexes

    info_file_path = os.path.join(assets_dir, tilemap_json['infoFile'])
    file = open(info_file_path, 'r')

    if not file:
        raise Exception(f"Failed to open file {info_file_path}")
    
    tilesets = {}

    for line in file:
        tile = line.split(' ')
        tile_name_split = tile[0].split('.')[0].split('_')
        tileset_name = '_'.join(tile_name_split[:-1])
        tile_index = int(tile_name_split[-1])
        tile_data = map(int, tile[1:]) # 6 values

        if tileset_name not in tilesets:
            if tileset_name in global_tileset_indexes:
                raise Exception(f"Tileset {tileset_name} is present in more than a one tilemap")
            tileset_index = len(global_tileset_indexes)
            global_tileset_indexes[tileset_name] = tileset_index
            tilemap_name = tilemap_json['name']
            tilesets[tileset_name] = {
                'tilemap_index': global_tilemap_indexes[tilemap_name],
                'tiles': [],
                'tileset_index': tileset_index,
                'tileset_name': tileset_name
            }

        tilesets[tileset_name]['tiles'].append({
            'index': tile_index,
            'data': tile_data
        })

    file.close()
    return tilesets.values()


def get_tilesets_data(tilesets):
    tilesets_data = b''
    for tileset in tilesets:
        tilesets_data += struct.pack('H', tileset['tilemap_index'])
        tiles = tileset['tiles']
        tilesets_data += struct.pack('B', len(tiles))
        for tile in tiles:
            tilesets_data += struct.pack('B', tile['index'])
            tilesets_data += struct.pack('6H', *tile['data'])
    
    return tilesets_data


def write_submodels_block(file, submodels_json, input_indexes):
    submodels_data = b''
    for [submodel_name, submodel] in submodels_json.items():
        name = submodel_name.encode('utf-8')
        name_length = len(name)
        tileset_index = global_tileset_indexes.get(submodel['tileset'], -1)
        if tileset_index == -1:
            print(global_tileset_indexes)
            raise Exception(f"Tileset {submodel['tileset']} not found")
        index_by = input_indexes.get(submodel.get('index_by', ''), 0)
        condition = input_indexes.get(submodel.get('condition', ''), 0)
        rot_by = input_indexes.get(submodel.get('rot_by', ''), 0xFFFF)
        condition_value = int(submodel.get('condition_value', 0))

        # temporary until bogies' rotation is calculated by track position
        if submodel.get('index_by', '') == '' and rot_by != 0xFFFF:
            index_by = 0xFFFF  

        submodels_data += struct.pack(
            'B', name_length) + name + struct.pack(
            'H', tileset_index) + struct.pack(
            'H', index_by) + struct.pack(
            'B', submodel.get('range', 120)) + struct.pack(
            'B', submodel.get('index_offset', 0)) + struct.pack(
            'H', condition) + struct.pack(
            'B', condition_value) + struct.pack(
            'H', rot_by) + struct.pack(
            'i', int(submodel.get('pos_x', 0) * _float_scale_)) + struct.pack(
            'i', int(submodel.get('pos_y', 0) * _float_scale_))

    file.write(b'SMOD')
    block_size = 2 + len(submodels_data)
    file.write(struct.pack('I', block_size))
    file.write(struct.pack('H', len(submodels_json)))
    file.write(submodels_data)


def write_views_block(file, views_json, submodel_indexes):
    views_data = b''
    for view in views_json:
        views_data += get_view_def_data(view, submodel_indexes)

    file.write(b'VIEW')
    block_size = 3 + len(views_data)
    file.write(struct.pack('I', block_size))
    file.write(struct.pack('H', 0))  # default indexByInputIndex
    file.write(struct.pack('B', len(views_json)))
    file.write(views_data)


def get_view_def_data(view_def_json, submodel_indexes):
    submodels_count = len(view_def_json['submodels'])
    view_def_data = struct.pack('H', submodels_count)
    for submodel_name in view_def_json['submodels']:
        submodel_index = submodel_indexes[submodel_name]
        view_def_data += struct.pack('H', submodel_index)
    return view_def_data


def write_binary_file(json_data, output_file_path, assets_dir):
    with open(output_file_path, 'wb') as file:
        file.write(b'SGEBMD')
        file.write(b'\x00\x00\x00\x00')  # placeholder for file size

        write_info_block(file, json_data['info'])
        input_indexes = get_input_indexes(json_data['submodels'])
        write_inputs_block(file, input_indexes)
        write_tilemaps_block(file, json_data['tilemaps'])
        write_tilesets_block(file, json_data['tilemaps'], assets_dir)
        write_submodels_block(file, json_data['submodels'], input_indexes)
        submodel_indexes = get_submodel_indexes(json_data['submodels'])
        write_views_block(file, json_data['views'], submodel_indexes)

        # Go back and write the actual size of the file
        file_size = file.tell() - 10  # subtract the header and initial size placeholder
        file.seek(6)
        file.write(struct.pack('I', file_size))


def replace_series_in_submodels(submodels_json, series):
    for series_name in series.keys():
        submodels_json.pop(series_name)

        series_size = series[series_name]['series_size']
        series_start = series[series_name]['series_start']

        for i in range(series_start, series_start + series_size):
            new_submodel = series[series_name].copy()
            new_submodel_name = series_name.format(i)

            # change every {} in entries that are strings to the current index
            for key, value in new_submodel.items():
                if isinstance(value, str):
                    new_submodel[key] = value.format(i)

            submodels_json[new_submodel_name] = new_submodel


def replace_series_in_view(view_json, series):
    submodels_list = view_json['submodels']

    for i, submodel_name in enumerate(submodels_list):
        if submodel_name in series:
            series_size = series[submodel_name]['series_size']
            series_start = series[submodel_name]['series_start']

            submodels_list.pop(i)
            for j in range(series_start, series_start + series_size):
                submodels_list.insert(i, submodel_name.format(j))
                i += 1


def resolve_series(json_data):
    submodels_json = json_data['submodels']
    views_json = json_data['views']

    series = {
        submodel_name: submodel
        for submodel_name, submodel in submodels_json.items()
        if submodel.get('series_size', 0) > 0
    }

    replace_series_in_submodels(submodels_json, series)

    for view_json in views_json:
        replace_series_in_view(view_json, series)


def __main__():
    parser = argparse.ArgumentParser(description="Convert JSON data to SGEBMD binary format")
    parser.add_argument("input_json", type=str, help="Input JSON file")
    parser.add_argument("output_bin", type=str, help="Output binary file")
    parser.add_argument("--assets-dir", type=str, help="Directory containing additional assets mentioned in the JSON")
    args = parser.parse_args()

    if not os.path.exists(args.input_json):
        print(f"Error: {args.input_json} does not exist.")
        return
    
    if not args.assets_dir:
        args.assets_dir = os.path.dirname(args.input_json)

    with open(args.input_json, 'r') as json_file:
        try:
            json_data = json.load(json_file)
        except json.JSONDecodeError as e:
            print(f"Error: Failed to parse JSON - {e}")
            return
        
    resolve_series(json_data)

    write_binary_file(json_data, args.output_bin, args.assets_dir)

    print("Done")


if __name__ == "__main__":
    __main__()
