typedef struct _ovm {
	t_object ob;
	t_critical lock;
	
	long inlet;
	void **proxies;
	int32_t nproxies;

	char *bytes;
	ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o,
        vm_f, vm_g, vm_x;

    t_symbol **typed_method_names;
    char **typed_method_types;
    int32_t *typed_method_ntypes;
    long typed_methods_count;
    long typed_methods_size;

} ovm;

t_class *ovm_class;
t_symbol *ps_FullPacket;
