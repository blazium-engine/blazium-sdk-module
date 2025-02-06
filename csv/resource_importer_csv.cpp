/**************************************************************************/
/*  resource_importer_csv.cpp                                             */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             BLAZIUM ENGINE                             */
/*                        https://http://blazium.app                      */
/**************************************************************************/
/* Copyright (c) 2024-present Blazium Engine contributors.                */
/* Copyright (c) 2024 Dragos Daian, Randolph William Aarseth II.          */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "resource_importer_csv.h"
#include "resource_csv.h"
#include "core/io/file_access.h"
#include "core/io/resource_saver.h"

String ResourceImporterCSV::get_importer_name() const {
	return "csv";
}

String ResourceImporterCSV::get_visible_name() const {
	return "CSV";
}

void ResourceImporterCSV::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("csv");
	p_extensions->push_back("tsv");
	p_extensions->push_back("psv");
}

String ResourceImporterCSV::get_save_extension() const {
	return "res";
}

String ResourceImporterCSV::get_resource_type() const {
	return "CSV";
}

bool ResourceImporterCSV::get_option_visibility(const String &p_path, const String &p_option, const HashMap<StringName, Variant> &p_options) const {
	return true;
}

int ResourceImporterCSV::get_preset_count() const {
	return 0;
}

String ResourceImporterCSV::get_preset_name(int p_idx) const {
	return "";
}

void ResourceImporterCSV::get_import_options(const String &p_path, List<ImportOption> *r_options, int p_preset) const {
	if (p_path.get_extension().contains("csv")) {
		r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "delimiter", PROPERTY_HINT_ENUM, "Comma,Semicolon,Tab,Pipe"), RowSeparator::ROW_SEPARATOR_COMMA));
	} else if (p_path.get_extension().contains("tsv")){
		r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "delimiter", PROPERTY_HINT_ENUM, "Comma,Semicolon,Tab,Pipe"), RowSeparator::ROW_SEPARATOR_TAB));
	} else if (p_path.get_extension().contains("psv")){
		r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "delimiter", PROPERTY_HINT_ENUM, "Comma,Semicolon,Tab,Pipe"), RowSeparator::ROW_SEPARATOR_PIPE));
	}
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "headers"), true));
}

Error ResourceImporterCSV::import(const String &p_source_file, const String &p_save_path, const HashMap<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	String delimiter;
	switch ((int)p_options["delimiter"]) {
		case 0:
			delimiter = ",";
			break;
		case 1:
			delimiter = ";";
			break;
		case 2:
			delimiter = "\t";
			break;
		case 3:
			delimiter = "|";
			break;
	}
	bool headers = p_options.get("headers");
	Ref<FileAccess> f = FileAccess::open(p_source_file, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(f.is_null(), ERR_INVALID_PARAMETER, "Cannot open file from path '" + p_source_file + "'.");
	
	Ref<CSV> csv;
	csv.instantiate();
	csv->headers = headers;
	csv->delimiter = delimiter;
	Error err = csv->load_file(p_source_file);
	if (err != OK) {
		ERR_PRINT("Failed to load CSV from path '" + p_source_file + "'.");
		return err;
	}
	err =  ResourceSaver::save(csv, p_save_path + ".res");
	if (err != OK) {
		ERR_PRINT("Failed to save CSV to path '" + p_save_path + "'.");
		return err;
	}
	r_gen_files->push_back(p_save_path + ".res");
	return OK;
}

ResourceImporterCSV::ResourceImporterCSV() {
}
