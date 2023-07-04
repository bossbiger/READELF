#include "..\inc\ELF32_class.h"
#include <iostream>

bool ELF_32::Load_elf_file(const char* file_path) {
	fopen_s(&elfile, file_path, "rb");
	if (elfile == nullptr) {
		return 0;
	}
	else {
		return 1
			;
	}
}
bool ELF_32::parse_elf_metadata()
{
	if (elfile == nullptr) {

		return 0;
	}
	ELF_METADATA = (ELF_32_METADATA*)malloc(sizeof(ELF_32_METADATA));
	if (fread(ELF_METADATA, sizeof(ELF_32_METADATA), 1, this->elfile)) {
		if (ELF_METADATA->e_ident[1] != 'E' || ELF_METADATA->e_ident[2] != 'L' || ELF_METADATA->e_ident[3] != 'F' || ELF_METADATA->e_ident[4] == 2) {
			free(ELF_METADATA);
			ELF_METADATA = nullptr;
			fclose(this->elfile);
			return 0;
		}
		else {
			return 1;
		}
	}
	return 0;
}
bool ELF_32::parse_shdrs()
{
	//check if the binary was loaded or not 
	if (this->elfile == nullptr) {
		return 0;
	}

	//check if the binary metadata was already parsed or not 
	if (this->ELF_METADATA == nullptr) {
		if (!this->parse_elf_metadata()) {
			return 0;
		}
	}
	//check if there is a sh_table or not 
	if (ELF_METADATA->e_shnum == 0) {
		return 0;
	}
	this->ELF_SHRD = (ELF_32_Shdr*)malloc(sizeof(ELF_32_Shdr) * ELF_METADATA->e_shnum);
	FILE* elfile_temp = elfile;
	fseek(elfile_temp, ELF_METADATA->e_shoff, SEEK_SET);
	if (fread(this->ELF_SHRD, sizeof(ELF_32_Shdr) * ELF_METADATA->e_shnum, 1, elfile_temp)) {
		return 1;
	}

	return 0;


}
bool ELF_32::parse_prog_hdrs()
{

	//Check if the binary is already loaded 
	if (this->elfile == nullptr) {
		return 0;
	}

	//check if the Elf metadata is already parsed or not 
	if (this->ELF_METADATA == nullptr) {
		if (!this->parse_elf_metadata()) {
			return 0;
		}
	}
	//check if there is any p_header 
	//in the normal case must be exist at least one pheader 
	if (ELF_METADATA->e_phnum == 0) {
		return 0;
	}
	int PROG_HDR_TABLE_SIZE = sizeof(ELF_32_PROG_HDR) * ELF_METADATA->e_phnum;
	ELF_PROGH = (ELF_32_PROG_HDR*)malloc(PROG_HDR_TABLE_SIZE);
	FILE* elfile_temp = this->elfile;
	fseek(elfile_temp, ELF_METADATA->e_phoff, SEEK_SET);
	if (fread(ELF_PROGH, PROG_HDR_TABLE_SIZE, 1, elfile_temp)) {
		return 1;
	}
	return 0;
}
bool ELF_32::parse()
{
	if (this->elfile == nullptr) {
		return 0;
	}
	if (this->ELF_METADATA == nullptr) {
		if (!this->parse_elf_metadata())
		return 0;
	}
	if (this->ELF_SHRD == nullptr) {
		this->parse_shdrs();
	}
	if (this->ELF_PROGH == nullptr) {
		if (!this->parse_prog_hdrs())
		return 0;
	}
	return 1;
}
int ELF_32::sections_num() {
	if (ELF_METADATA == nullptr) {
		return 0;
	}
	return ELF_METADATA->e_shnum; 
}
int ELF_32::segment_num() {
	if (ELF_METADATA == nullptr) {
		return 0;
	}
	return ELF_METADATA->e_phnum;
}
bool ELF_32::parse_section_info(int sec_num, ELF_32_Sinfo* shd_ptr)
{
	if (ELF_METADATA == nullptr ){
		if (!parse_elf_metadata()) {
			return false;
		}
	}
	if (ELF_SHRD == nullptr) {
		if (!parse_shdrs()) {
			return false; 
		}
	}
	if (sec_num > ELF_METADATA->e_shnum - 1 || shd_ptr==nullptr ) {
		return false;
	}
	char buffer[30];
	Elf32_Off shstoff = ELF_SHRD[ELF_METADATA->e_shstrndx].sh_offset;
	Elf32_Off sname_offset = ELF_SHRD[sec_num].sh_name + shstoff;
	FILE* f = elfile; 
	fseek(f, sname_offset, SEEK_SET);
	fread(buffer, 30, 1, f);
	for (int i = 0; i < 30; i++) {
		if (buffer[i] != '/0') {
			shd_ptr->sh_name[i] = buffer[i]; 
		}
	}
	shd_ptr->sh_addr = ELF_SHRD[sec_num].sh_addr; 
	shd_ptr->sh_offset = ELF_SHRD[sec_num].sh_offset; 
	shd_ptr->sh_size = ELF_SHRD[sec_num].sh_size;
	shd_ptr->sh_entsize = ELF_SHRD[sec_num].sh_entsize;
	shd_ptr->sh_info = ELF_SHRD[sec_num].sh_info;
	shd_ptr->sh_link = ELF_SHRD[sec_num].sh_link;
	shd_ptr->sh_addralign = ELF_SHRD[sec_num].sh_addralign;
	switch (ELF_SHRD[sec_num].sh_type) {
	case 0:
		shd_ptr->sh_type = "SHT_NULL";
		break; 
	case 1:
		shd_ptr->sh_type = "SHT_PROGBITS";
		break;
	case 2:
		shd_ptr->sh_type = "SHT_SYMTAB";
		break;
	case 3:
		shd_ptr->sh_type = "SHT_STRTAB";
		break;
	case 4:
		shd_ptr->sh_type = "SHT_RELA";
		break;
	case 5:
		shd_ptr->sh_type = "SHT_HASH";
		break;
	case 6:
		shd_ptr->sh_type = "SHT_DYNAMIC";
		break;
	case 7:
		shd_ptr->sh_type = "SHT_NOTE";
		break;
	case 8:
		shd_ptr->sh_type = "SHT_NOBITS";
		break;
	case 9:
		shd_ptr->sh_type = "SHT_REL";
		break;
	case 10:
		shd_ptr->sh_type = "SHT_SHLIB";
		break;
	case 11:
		shd_ptr->sh_type = "SHT_DYNSYM";
		break;
	case 14:
		shd_ptr->sh_type = "SHT_INIT_ARRAY";
		break;
	case 15:
		shd_ptr->sh_type = "SHT_FINI_ARRAY";
		break;
	case 16:
		shd_ptr->sh_type = "SHT_PREINIT_ARRAY";
		break;
	case 17:
		shd_ptr->sh_type = "SHT_GROUP";
		break;
	case 18:
		shd_ptr->sh_type = "SHT_SYMTAB_SHNDX";
		break;
	default:
		shd_ptr->sh_type = "UNDIFEND"; 
		break;
	}
	shd_ptr->sh_flags = ELF_SHRD[sec_num].sh_flags; 
	return true; 
}
bool ELF_32::parse_segment_info(int seg_num, ELF_32_PROG_HDR_INFO* phd_ptr)
{
	if (this->ELF_PROGH == nullptr) {
		if (!this->parse_prog_hdrs())
			return 0;
	}
	if (seg_num > ELF_METADATA->e_phnum - 1 ) {
		return 0;
	}
	if (phd_ptr == nullptr) {
		return 0;
	}
	switch (ELF_PROGH[seg_num].p_type) {
	case PT_NULL:
		phd_ptr->p_type = "NULL";
		break;
	case PT_LOAD:
		phd_ptr->p_type = "PT_LOAD";
		break;
	case PT_DYNAMIC:
		phd_ptr->p_type = "PT_DYNAMIC";
		break;
	case PT_INTERP:
		phd_ptr->p_type = "PT_INTERP";
		break;
	case PT_NOTE:
		phd_ptr->p_type = "PT_NOTE";
		break;
	case PT_SHLIB:
		phd_ptr->p_type = "PT_SHLIB";
		break;
	case PT_PHDR:
		phd_ptr->p_type = "PT_PHDR";
		break;
	case PT_LOPROC:
		phd_ptr->p_type = "PT_LOPROC";
		break;
	case PT_HIPROC:
		phd_ptr->p_type = "PT_HIPROC";
	default:
		break;
	}
	phd_ptr->p_align = ELF_PROGH[seg_num].p_align;
	phd_ptr->p_filesz = ELF_PROGH[seg_num].p_filesz;
	phd_ptr->p_memsz = ELF_PROGH[seg_num].p_memsz;
	phd_ptr->p_offset = ELF_PROGH[seg_num].p_offset;
	phd_ptr->p_paddr = ELF_PROGH[seg_num].p_paddr;
	phd_ptr->p_vaddr = ELF_PROGH[seg_num].p_vaddr;
	phd_ptr->p_flags = ELF_PROGH[seg_num].p_flags;


	return 1;
}
bool ELF_32::parse_elf_metadata(ELF_32_INFO* ELf_metada_ptr) {
	if (ELF_METADATA == nullptr) {
		return false; 
	}
	for (int i = 0; i < EI_NIDENT; i++) {
		ELf_metada_ptr->e_ident[i] = ELF_METADATA->e_ident[i] ;
	}
	switch (ELF_METADATA->e_type)
	{
	case 0:
		break;
	case 1:
		ELf_metada_ptr->e_type = "ET_REL";
		break;
	case 2:
		ELf_metada_ptr->e_type = "ET_EXEC";
		break;
	case 3:
		ELf_metada_ptr->e_type = "ET_DYN";
		break;
	case 4:
		ELf_metada_ptr->e_type = "ET_CORE";
		break;
	case 0xfe00:
		ELf_metada_ptr->e_type = "ET_LOOS";
		break;
	case 0xfeff:
		ELf_metada_ptr->e_type = "ET_HIOS";
		break;
	case 0xff00:
		ELf_metada_ptr->e_type = "ET_LOPROC";
		break;
	case 0xffff:
		ELf_metada_ptr->e_type = "ET_HIPROC";
		break;

	default:
		break;
	}
	switch (ELF_METADATA->e_machine)
	{
	case 0:
		ELf_metada_ptr->e_machine = "EM_NONE"; 
		break;
	case 1:
		ELf_metada_ptr->e_machine = "EM_M32";
		break;
	case 2:
		ELf_metada_ptr->e_machine = "EM_SPARC";
		break;
	case 3:
		ELf_metada_ptr->e_machine = "EM_386";
		break;
	case 4:
		ELf_metada_ptr->e_machine = "EM_68K";
		break;
	case 5:
		ELf_metada_ptr->e_machine = "EM_88K";
		break;
	case 6:
		ELf_metada_ptr->e_machine = "EM_IAMCU";
		break;
	case 7:
		ELf_metada_ptr->e_machine = "EM_860";
		break;
	case 8:
		ELf_metada_ptr->e_machine = "EM_MIPS";
		break;
	default:
		ELf_metada_ptr->e_machine = "OTHER"; 
		break;
	}
	ELf_metada_ptr->e_version = "1"; 
	ELf_metada_ptr->e_entry = ELF_METADATA->e_entry; 
	ELf_metada_ptr->e_phoff = ELF_METADATA->e_phoff;
	ELf_metada_ptr->e_shoff = ELF_METADATA->e_shoff;
	ELf_metada_ptr->e_flags = ELF_METADATA->e_flags;
	ELf_metada_ptr->e_ehsize = ELF_METADATA->e_ehsize;
	ELf_metada_ptr->e_phentsize = ELF_METADATA->e_phentsize;
	ELf_metada_ptr->e_phnum = ELF_METADATA->e_phnum;
	ELf_metada_ptr->e_shentsize = ELF_METADATA->e_shentsize;
	ELf_metada_ptr->e_shnum = ELF_METADATA->e_shnum;
	ELf_metada_ptr->e_shstrndx = ELF_METADATA->e_shstrndx;
	return true; 
}

