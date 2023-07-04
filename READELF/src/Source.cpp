#include<iostream>
#include "..\inc\ELF32_class.h"

bool ELF_METADATA(ELF_32* ELF_FILE_PTR) {
	ELF_32_INFO info;
	ELF_FILE_PTR->parse_elf_metadata(&info);
	ELF_FILE_PTR->parse_elf_metadata(&info); 
	std::cout << "ELF HEADER : " << std::hex<<info.e_ident << std::endl;
	std::cout << "MACHINE : " << info.e_machine << std::endl; 
	std::cout << "VERSION  : " << info.e_version <<" (DEFAULT)" << std::endl;
	std::cout << "Entry point address  : 0x" <<std::hex<< info.e_entry << std::endl;
	std::cout << "Start of program headers : 0x" << info.e_phoff << std::endl;
	std::cout << "Start of section headers : 0x" << info.e_shoff << std::endl;
	std::cout << "Flags  : " << info.e_flags << std::endl;
	std::cout <<std::hex<< "Size of this header  : 0x" << info.e_ehsize << std::endl;
	std::cout << "Size of program headers  : 0x" << info.e_phentsize << std::endl;
	std::cout << "Number of program headers   : 0x" << info.e_phnum << std::endl;
	std::cout << "Size of section headers   : 0x" << info.e_shentsize << std::endl;
	std::cout << "Number of section headers   : " << info.e_shnum << std::endl;
	std::cout << "Section header string table index  : 0x" << info.e_shstrndx << std::endl;
	return true; 

}
bool ELF_SECTION_INFO(ELF_32* ELF_FILE_PTR ,int sec_num) {
	if (ELF_FILE_PTR->sections_num() == 0 ) {
		std::cout << "There is No Section" << std::endl; 
		return false;
	}
	if (sec_num > ELF_FILE_PTR->sections_num()) {
		return false; 
	}
	else {
		ELF_32_Sinfo section_info; 
		ELF_FILE_PTR->parse_section_info(sec_num, &section_info);  
		std::cout << "SECTION NAME:  " << section_info.sh_name << std::endl; 
		std::cout << "SECTION TYPE :  " << section_info.sh_type << std::endl; 
		std::cout << "SECTION SIZE  : 0x" << section_info.sh_size << std::endl; 
		std::cout << "SECTION OFFSET  : 0x" << section_info.sh_offset << std::endl; 
		std::cout << "SECTION LINK :  0x" << section_info.sh_link << std::endl; 
		std::cout << "SECTION INFO :  0x" << section_info.sh_info << std::endl; 
		switch (section_info.sh_flags)
		{
		case 0x1:
			std::cout << "SECTION FLAGS : SHF_WRITE  " << std::endl;
			break;
		case 0x2:
			std::cout << "SECTION FLAGS : SHF_ALLOC " << std::endl;
			break;
		case 0x4:
			std::cout << "SECTION FLAGS : SHF_EXECINSTR " << std::endl;
			break;
		case 0x3:
			std::cout << "SECTION FLAGS : SHF_ALLOC | SHF_WRITE " << std::endl;
			break;
		case 0x5:
			std::cout << "SECTION FLAGS : SHF_EXECINSTR | SHF_WRITE " << std::endl;
			break;
		case 0x6:
			std::cout << "SECTION FLAGS : SHF_EXECINSTR | SHF_ALLOC " << std::endl;
			break;
		default:
			break;
		}
		std::cout << "SECTION ENTSIZE :  0x" << section_info.sh_entsize << std::endl; 
		std::cout << "SECTION ADDRALIGN :  0x" << section_info.sh_addralign << std::endl; 
		std::cout << "SECTION ADDR  :  0x" << section_info.sh_addr << std::endl; 
		return true; 
	}
}
bool ELF_SEGMENT_INFO(ELF_32* ELF_FILE_PTR, int seg_num) {
	if (ELF_FILE_PTR->segment_num() == 0) {
		std::cout << "There is No Section" << std::endl;
		return false;
	}
	if (seg_num > ELF_FILE_PTR->segment_num()) {
		return false; 
	}
	else {
		ELF_32_PROG_HDR_INFO segment_info;
		ELF_FILE_PTR->parse_segment_info(seg_num, &segment_info);
		std::cout << "SEGMENT TYPE :  " << segment_info.p_type << std::endl;
		if (segment_info.p_flags == 0x1) {
			std::cout << "FLAGS : PF_X" << std::endl;
		}
		if (segment_info.p_flags == 0x2) {
			std::cout << "FLAGS : PF_W" << std::endl;
		}
		if (segment_info.p_flags == 0x4) {
			std::cout << "FLAGS : PF_R" << std::endl;
		}
		if (segment_info.p_flags == 0x3) {
			std::cout << "FLAGS : PF_X| PF_W" << std::endl;
		}
		if (segment_info.p_flags == 0x5) {
			std::cout << "FLAGS : PF_R| PF_X" << std::endl;
		}
		if (segment_info.p_flags == 0x6) {
			std::cout << "FLAGS : PF_R| PF_W" << std::endl;
		}

		std::cout << "SEGMENT MEMORY SIZE   : 0x" << segment_info.p_memsz << std::endl;
		std::cout << "SEGMENT OFFSET :  0x" << segment_info.p_offset << std::endl;
		std::cout << "SEGMENT PHYSICAL ADDRESS  :  0x" << segment_info.p_paddr << std::endl;
		std::cout << "SEGMENT FILE SIZE  :  0x" << segment_info.p_filesz << std::endl;
		std::cout << "SEGMENT VIRTUAL ADDESS  :  0x" << segment_info.p_vaddr << std::endl;
		return true;
	}
}

int main() {
	ELF_32 file; 
	char name[60];
	std::cout << "Enter File name " << std::endl; 
	std::cin >> name; 
	if (file.Load_elf_file(name) == true) {
		std::cout << "Reading file (+)" << std::endl;
		std::cout << std::endl;
	}
	else {
		std::cout << "Reading File FAILED [-]" << std::endl; 
		std::cout << std::endl; 
		return 0;
	}
	if (file.parse()== true ) {
		std::cout << "Parsed (+) "<<std::endl; 
	}
	else {
		std::cout << "Parsing File Failed (-)" << std::endl; 
		int W; 
		std::cin>> W;
		return 0; 
	}
//PRINTING ELF METADATA 
	ELF_METADATA(&file); 
	std::cout << "______________________________________________________" << std::endl;
	std::cout << "______________________________________________________" << std::endl;
	std::cout << std::endl; 
	std::cout << std::endl; 
	
//PRINTING SECTION HEADER TABLE 	
	
	
	std::cout << "SECTIONS INFO   : " << std::endl;
	for (int i = 0; i < file.sections_num(); i++) {
		std::cout << std::endl;
		ELF_SECTION_INFO(&file, i);
		std::cout << std::endl;
		std::cout << std::endl;

	}
	
//PRINTING SEGMENT HEADER TABLE 	

	std::cout << "______________________________________________________" << std::endl;
	std::cout << "______________________________________________________" << std::endl;
	std::cout << "SEGMENTS INFO  : " << std::endl;
	for (int i = 0; i < file.segment_num(); i++) {
		std::cout << std::endl;
		std::cout << std::endl;
		ELF_SEGMENT_INFO(&file, i);
	}
	



	int y; 
	std::cin >> y; 

}
