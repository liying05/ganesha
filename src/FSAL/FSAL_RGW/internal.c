/*
 * Copyright © Red Hat, 2015
 * Author: ORit Wasserman <owasserm@redhat.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * -------------
 */

/**
 * @file   internal.c
 * @brief Internal definitions for the RGW FSAL
 *
 * This file includes internal function definitions, constants, and
 * variable declarations used to impelment the RGW FSAL, but not
 * exposed as part of the API.
 */

#include <sys/stat.h>
#include "fsal_types.h"
#include "fsal.h"
#include "fsal_convert.h"
#include "FSAL/fsal_commonlib.h"

#define RGW_INTERNAL_C
#include "internal.h"


struct rgw_fsal_module RGWFSM;


/**
 * @brief FSAL status from RGW error
 *
 * This function returns a fsal_status_t with the FSAL error as the
 * major, and the posix error as minor.	 (RGW's error codes are just
 * negative signed versions of POSIX error codes.)
 *
 * @param[in] rgw_errorcode RGW error (negative Posix)
 *
 * @return FSAL status.
 */

fsal_status_t rgw2fsal_error(const int rgw_errorcode)
{
	fsal_status_t status;
	status.minor = -rgw_errorcode;

	switch (-rgw_errorcode) {

	case 0:
		status.major = ERR_FSAL_NO_ERROR;
		break;

	case EPERM:
		status.major = ERR_FSAL_PERM;
		break;

	case ENOENT:
		status.major = ERR_FSAL_NOENT;
		break;

	case ECONNREFUSED:
	case ECONNABORTED:
	case ECONNRESET:
	case EIO:
	case ENFILE:
	case EMFILE:
	case EPIPE:
		status.major = ERR_FSAL_IO;
		break;

	case ENODEV:
	case ENXIO:
		status.major = ERR_FSAL_NXIO;
		break;

	case EBADF:
		/**
		 * @todo: The EBADF error also happens when file is
		 *	  opened for reading, and we try writting in
		 *	  it.  In this case, we return
		 *	  ERR_FSAL_NOT_OPENED, but it doesn't seems to
		 *	  be a correct error translation.
		 */
		status.major = ERR_FSAL_NOT_OPENED;
		break;

	case ENOMEM:
		status.major = ERR_FSAL_NOMEM;
		break;

	case EACCES:
		status.major = ERR_FSAL_ACCESS;
		break;

	case EFAULT:
		status.major = ERR_FSAL_FAULT;
		break;

	case EEXIST:
		status.major = ERR_FSAL_EXIST;
		break;

	case EXDEV:
		status.major = ERR_FSAL_XDEV;
		break;

	case ENOTDIR:
		status.major = ERR_FSAL_NOTDIR;
		break;

	case EISDIR:
		status.major = ERR_FSAL_ISDIR;
		break;

	case EINVAL:
		status.major = ERR_FSAL_INVAL;
		break;

	case EFBIG:
		status.major = ERR_FSAL_FBIG;
		break;

	case ENOSPC:
		status.major = ERR_FSAL_NOSPC;
		break;

	case EMLINK:
		status.major = ERR_FSAL_MLINK;
		break;

	case EDQUOT:
		status.major = ERR_FSAL_DQUOT;
		break;

	case ENAMETOOLONG:
		status.major = ERR_FSAL_NAMETOOLONG;
		break;

	case ENOTEMPTY:
		status.major = ERR_FSAL_NOTEMPTY;
		break;

	case ESTALE:
		status.major = ERR_FSAL_STALE;
		break;

	case EAGAIN:
	case EBUSY:
		status.major = ERR_FSAL_DELAY;
		break;

	default:
		status.major = ERR_FSAL_SERVERFAULT;
		break;
	}

	return status;
}

/**
 * @brief Convert a struct stat from RGW to a struct attrlist
 *
 * This function writes the content of the supplied struct stat to the
 * struct fsalsattr.
 *
 * @param[in]  buffstat Stat structure
 * @param[out] fsalattr FSAL attributes
 */

void rgw2fsal_attributes(const struct stat *buffstat,
			  struct attrlist *fsalattr)
{
	FSAL_CLEAR_MASK(fsalattr->mask);

	/* Fills the output struct */
	fsalattr->type = posix2fsal_type(buffstat->st_mode);
	FSAL_SET_MASK(fsalattr->mask, ATTR_TYPE);

	fsalattr->filesize = buffstat->st_size;
	FSAL_SET_MASK(fsalattr->mask, ATTR_SIZE);

	fsalattr->fsid = posix2fsal_fsid(buffstat->st_dev);
	FSAL_SET_MASK(fsalattr->mask, ATTR_FSID);

	fsalattr->fileid = buffstat->st_ino;
	FSAL_SET_MASK(fsalattr->mask, ATTR_FILEID);

	fsalattr->mode = unix2fsal_mode(buffstat->st_mode);
	FSAL_SET_MASK(fsalattr->mask, ATTR_MODE);

	fsalattr->numlinks = buffstat->st_nlink;
	FSAL_SET_MASK(fsalattr->mask, ATTR_NUMLINKS);

	fsalattr->owner = buffstat->st_uid;
	FSAL_SET_MASK(fsalattr->mask, ATTR_OWNER);

	fsalattr->group = buffstat->st_gid;
	FSAL_SET_MASK(fsalattr->mask, ATTR_GROUP);

	fsalattr->atime = posix2fsal_time(buffstat->st_atime, 0);
	FSAL_SET_MASK(fsalattr->mask, ATTR_ATIME);

	fsalattr->ctime = posix2fsal_time(buffstat->st_ctime, 0);
	FSAL_SET_MASK(fsalattr->mask, ATTR_CTIME);

	fsalattr->mtime = posix2fsal_time(buffstat->st_mtime, 0);
	FSAL_SET_MASK(fsalattr->mask, ATTR_MTIME);

	fsalattr->chgtime =
	    posix2fsal_time(MAX(buffstat->st_mtime, buffstat->st_ctime), 0);
	fsalattr->change = fsalattr->chgtime.tv_sec;
	FSAL_SET_MASK(fsalattr->mask, ATTR_CHGTIME);

	fsalattr->spaceused = buffstat->st_blocks * S_BLKSIZE;
	FSAL_SET_MASK(fsalattr->mask, ATTR_SPACEUSED);

	fsalattr->rawdev = posix2fsal_devt(buffstat->st_rdev);
	FSAL_SET_MASK(fsalattr->mask, ATTR_RAWDEV);
}

/**
 * @brief Construct a new filehandle
 *
 * This function constructs a new RGW FSAL object handle and attaches
 * it to the export.  After this call the attributes have been filled
 * in and the handle is up-to-date and usable.
 *
 * @param[in]  export Export on which the object lives
 * @param[in]  rgw_fh Concise representation of the object name,
 *                    in RGW notation
 * @param[inout] st   Object attributes
 * @param[out] obj    Object created
 *
 * @return 0 on success, negative error codes on failure.
 */

int construct_handle(struct rgw_export *export,
		     struct rgw_file_handle *rgw_fh,
		     struct stat *st,
		     struct rgw_handle **obj)

{
	/* Poitner to the handle under construction */
	struct rgw_handle *constructing = NULL;
	*obj = NULL;

	constructing = gsh_calloc(1, sizeof(struct rgw_handle));
	if (constructing == NULL)
		return -ENOMEM;

	constructing->rgw_fh = rgw_fh;
	constructing->up_ops = export->export.up_ops; /* XXXX going away */
	constructing->handle.attrs = &constructing->attributes;
	rgw2fsal_attributes(st, &constructing->attributes);

	fsal_obj_handle_init(&constructing->handle, &export->export,
			     constructing->attributes.type);
	handle_ops_init(&constructing->handle.obj_ops);
	constructing->export = export;

	*obj = constructing;

	return 0;
}
