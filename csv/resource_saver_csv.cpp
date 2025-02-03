#include "resource_saver_csv.h"
#include "resource_csv.h"
#include "core/io/file_access.h"

Error ResourceFormatSaverCSV::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	Ref<CSV> csv = p_resource;
	ERR_FAIL_COND_V(csv.is_null(), ERR_INVALID_PARAMETER);
	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);
	if (file.is_null()) {
		return ERR_CANT_CREATE;
	}
	TypedArray<Dictionary> rows = csv->get_rows();
	String header_row;
	String separator = ",";
	if (rows.size() > 0) {
		List<Variant> keys;
		Dictionary header = rows[0];
		header.get_key_list(&keys);
		for (int i = 0; i < keys.size(); i++) {
			String key = keys.get(i);
			header_row += key;
			if (i < keys.size() - 1) {
				header_row += separator;
			}
		}
		file->store_line(header_row);
		for (int i = 0; i < rows.size(); i++) {
			Dictionary row = rows[i];
			String row_string = "";
			for (int j = 0; j < keys.size(); j++) {
				Variant value = row[keys.get(j)];
				row_string += value.to_json_string();
				if (j < keys.size() - 1) {
					row_string += separator;
				}
			}
			file->store_line(row_string);
		}
	}
	return OK;
}

void ResourceFormatSaverCSV::get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const {
	if (Object::cast_to<CSV>(p_resource.ptr())) {
		p_extensions->push_back("csv");
	}
}

bool ResourceFormatSaverCSV::recognize(const Ref<Resource> &p_resource) const {
	return Object::cast_to<CSV>(p_resource.ptr()) != nullptr;
}
