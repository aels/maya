#include "maya.h"
#include "dwarf/dwarf.h"
#include "dwarf/libdwarf.h"

#define UNDEF_VAL 2000
#define SAME_VAL 2001
#define CFA_VAL 2002


struct fde_func_data {
	uint64_t addr;
	size_t size;
};

void print_fde_instrs(Dwarf_Debug, Dwarf_Fde, int, Dwarf_Error);
struct fde_func_data * get_func_data(Dwarf_Debug dbg, Dwarf_Fde fde, int fdenum);

struct fde_func_data * parse_frame_data(Dwarf_Debug dbg)
{
	Dwarf_Error error;
    	Dwarf_Signed cie_element_count = 0;
    	Dwarf_Signed fde_element_count = 0;
    	Dwarf_Signed fde_count;
	Dwarf_Cie *cie_data = 0;
    	Dwarf_Fde *fde_data = 0;
    	int res = DW_DLV_ERROR;
	Dwarf_Signed fdenum = 0;
	struct fde_func_data *func_data, *fdp;

	res = dwarf_get_fde_list_eh(dbg, &cie_data, &cie_element_count, &fde_data, &fde_element_count, &error);
    	if(res == DW_DLV_NO_ENTRY) {
   		printf("No frame data present ");
        	return NULL;
    	}

    	if(res == DW_DLV_ERROR) {
        	printf("Error reading frame data ");
        	return NULL;
    	}
	
	func_data = (struct fde_func_data *)malloc(sizeof(*func_data) * fde_element_count);
	if (func_data == NULL) {
		perror("malloc");
		return NULL;
	}
	
	fdp = func_data;
	for(fdenum = 0; fdenum < fde_element_count; ++fdenum, func_data++) {
        	Dwarf_Cie cie = 0;
        	res = dwarf_get_cie_of_fde(fde_data[fdenum],&cie,&error);
        	if(res != DW_DLV_OK) {
            		printf("Error accessing fdenum %" DW_PR_DSd
                	" to get its cie\n",fdenum);
            		return NULL;
        	}
        	func_data = get_func_data(dbg, fde_data[fdenum], fdenum);
		printf("Function size: %d Function Addr: %lx\n", (int)func_data->size, func_data->addr);
	}

	dwarf_fde_cie_list_dealloc(dbg, cie_data, cie_element_count, fde_data, fde_element_count);
   
	return fdp;
}


struct fde_func_data * get_func_data(Dwarf_Debug dbg, Dwarf_Fde fde, int fdenum)
{
	int res;
	Dwarf_Error error;
	Dwarf_Unsigned func_length = 0;
	Dwarf_Unsigned fde_byte_length = 0;
	Dwarf_Off cie_offset = 0;
	Dwarf_Off fde_offset = 0;
	Dwarf_Addr lowpc = 0;
	Dwarf_Signed cie_index = 0;
	Dwarf_Ptr fde_bytes;
	struct fde_func_data *func_data = malloc(sizeof(*func_data));
	
	
	res = dwarf_get_fde_range(fde, &lowpc, &func_length, &fde_bytes, &fde_byte_length, 
				  &cie_offset, &cie_index, &fde_offset, &error);
	if (res != DW_DLV_OK) {
		fprintf(stderr, "Failed to get fde range\n");
		return NULL;
	}
		
	func_data->addr = lowpc;
	func_data->size = func_length;

	return func_data;
}


size_t get_all_functions(const char *filepath, struct fde_func_data **funcs)
{
	int fd;
	int res = DW_DLV_ERROR;
	int regtabrulecount = 0;
	Dwarf_Debug dbg;
	Dwarf_Error error;
	Dwarf_Ptr errarg = 0;
	Dwarf_Handler errhand = 0;
        Dwarf_Signed cie_element_count = 0;
        Dwarf_Signed fde_element_count = 0;
        Dwarf_Signed fde_count;
        Dwarf_Cie *cie_data = 0;
        Dwarf_Fde *fde_data = 0;


	if ((fd = open(filepath, O_RDONLY)) < 0) {
		perror("open");
		exit(-1);
	}

	if ((res = dwarf_init(fd, /*DW_DLC_REA*/ 0, errhand, errarg, &dbg, &error)) != DW_DLV_OK) {
		fprintf(stderr, "dwarf_init() failed\n");
		exit(-1);
	}

    	regtabrulecount = 1999;
    	dwarf_set_frame_undefined_value(dbg, UNDEF_VAL);
    	dwarf_set_frame_rule_initial_value(dbg, UNDEF_VAL);
    	dwarf_set_frame_same_value(dbg, SAME_VAL);
    	dwarf_set_frame_cfa_value(dbg, CFA_VAL);
    	dwarf_set_frame_rule_table_size(dbg, regtabrulecount);
	
	res = dwarf_get_fde_list_eh(dbg, &cie_data, &cie_element_count, &fde_data, &fde_element_count, &error);
        if(res == DW_DLV_NO_ENTRY) {
                printf("No frame data present ");
                return NULL;
        }

	if ((*funcs = parse_frame_data(dbg)) == NULL) {
		fprintf(stderr, "parse_frame_data() failed\n");
		return -1;
	}

	res = dwarf_finish(dbg,&error);
    	
	if(res != DW_DLV_OK) 
        	fprintf(stderr, "dwarf_finish failed!\n");

	close(fd);
    	
	return fde_element_count;
}


	
	
