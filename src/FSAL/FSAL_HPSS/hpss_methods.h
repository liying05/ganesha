/*
 * vim:noexpandtab:shiftwidth=8:tabstop=8:
 *
 * DISCLAIMER
 * ----------
 * This file is part of FSAL_HPSS.
 * FSAL HPSS provides the glue in-between FSAL API and HPSS CLAPI
 * You need to have HPSS installed to properly compile this file.
 *
 * Linkage/compilation/binding/loading/etc of HPSS licensed Software
 * must occur at the HPSS partner's or licensee's location.
 * It is not allowed to distribute this software as compiled or linked
 * binaries or libraries, as they include HPSS licensed material.
 * -------------
 */

/* VFS methods for handles
 */

/* private helpers from export
 */

struct hpssfsal_export_context *hpss_get_root_pvfs(struct fsal_export *exp_hdl);

/* method proto linkage to handle.c for export
 */

fsal_status_t hpss_lookup_path(struct fsal_export *exp_hdl,
			       char *path,
			       struct fsal_obj_handle **handle);

fsal_status_t hpss_create_handle(struct fsal_export *exp_hdl,
				 struct gsh_buffdesc *hdl_desc,
				 struct fsal_obj_handle **handle);

/* methods from main needed in handle
 */
struct fsal_staticfsinfo_t *hpss_staticinfo(struct fsal_module *hdl);
struct hpss_specific_initinfo *hpss_specific_initinfo(struct fsal_module *hdl);
void hpss_handle_ops_init(struct fsal_obj_ops *ops);

/*
 * VFS internal object handle
 * handle is a pointer because
 *  a) the last element of file_handle is a char[] meaning variable len...
 *  b) we cannot depend on it *always* being last or being the only
 *     variable sized struct here...  a pointer is safer.
 * wrt locks, should this be a lock counter??
 */

static inline bool vfs_unopenable_type(object_file_type_t type)
{
	if ((type == SOCKET_FILE) ||
	    (type == CHARACTER_FILE) ||
	    (type == BLOCK_FILE))
		return true;
	else
		return false;
}


/* I/O management */
fsal_status_t hpss_open(struct fsal_obj_handle *obj_hdl,
			fsal_openflags_t openflags);
fsal_status_t hpss_commit(struct fsal_obj_handle *obj_hdl,
			  off_t offset,
			  size_t len);
fsal_openflags_t hpss_status(struct fsal_obj_handle *obj_hdl);
fsal_status_t hpss_read(struct fsal_obj_handle *obj_hdl,
			uint64_t offset,
			size_t buffer_size,
			void *buffer,
			size_t *read_amount,
			bool *end_of_file);
fsal_status_t hpss_write(struct fsal_obj_handle *obj_hdl,
			 uint64_t offset,
			 size_t buffer_size,
			 void *buffer,
			 size_t *write_amount,
			 bool *fsal_stable);
fsal_status_t hpss_share_op(struct fsal_obj_handle *obj_hdl,
			    void *p_owner,
			    fsal_share_param_t request_share);
fsal_status_t hpss_close(struct fsal_obj_handle *obj_hdl);
fsal_status_t hpss_lru_cleanup(struct fsal_obj_handle *obj_hdl,
			       lru_actions_t requests);

/* extended attributes management */
fsal_status_t hpss_list_ext_attrs(struct fsal_obj_handle *obj_hdl,
				  unsigned int cookie,
				  fsal_xattrent_t *xattrs_tab,
				  unsigned int xattrs_tabsize,
				  unsigned int *p_nb_returned,
				  int *end_of_list);
fsal_status_t hpss_getextattr_id_by_name(struct fsal_obj_handle *obj_hdl,
					 const char *xattr_name,
					 unsigned int *pxattr_id);
fsal_status_t hpss_getextattr_value_by_name(struct fsal_obj_handle *obj_hdl,
					    const char *xattr_name,
					    caddr_t buffer_addr,
					    size_t buffer_size,
					    size_t *p_output_size);
fsal_status_t hpss_getextattr_value_by_id(struct fsal_obj_handle *obj_hdl,
					  unsigned int xattr_id,
					  caddr_t buffer_addr,
					  size_t buffer_size,
					  size_t *p_output_size);
fsal_status_t hpss_setextattr_value(struct fsal_obj_handle *obj_hdl,
				    const char *xattr_name,
				    caddr_t buffer_addr,
				    size_t buffer_size,
				    int create);
fsal_status_t hpss_setextattr_value_by_id(struct fsal_obj_handle *obj_hdl,
					  unsigned int xattr_id,
					  caddr_t buffer_addr,
					  size_t buffer_size);
fsal_status_t hpss_getextattr_attrs(struct fsal_obj_handle *obj_hdl,
				    unsigned int xattr_id,
				    struct attrlist *p_attrs);
fsal_status_t hpss_remove_extattr_by_id(struct fsal_obj_handle *obj_hdl,
					unsigned int xattr_id);
fsal_status_t hpss_remove_extattr_by_name(struct fsal_obj_handle *obj_hdl,
					  const char *xattr_name);
fsal_status_t hpss_lock_op(struct fsal_obj_handle *obj_hdl,
			   void *p_owner,
			   fsal_lock_op_t lock_op,
			   fsal_lock_param_t *request_lock,
			   fsal_lock_param_t *conflicting_lock);
