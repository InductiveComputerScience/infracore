#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>

#include <Disk.h>

// http://asm.sourceforge.net/syscall.html
ssize_t sys_read(unsigned int fd, char *buf, size_t count);
ssize_t sys_write(unsigned int fd, const char *buf, size_t count);
int sys_open(const char *filename, int flags, int mode);
void sys_close(unsigned int fd);
int sys_fsync(unsigned int fd);
off_t sys_lseek(unsigned int fd, off_t offset, unsigned int origin);

struct DiskStructureLinux{
	int diskFD;
	sem_t disksem;
	char *diskname;
	int64_t entries;
	int64_t entrySize;
};

typedef struct DiskStructureLinux DiskStructureLinux;

int CreateLinuxFile(DiskStructure **disk, char *disksrc, int64_t entries, int64_t entrySize){
	int success;
	DiskStructureLinux *diskS;

	*disk = malloc(sizeof(DiskStructure));
	diskS = malloc(sizeof(DiskStructureLinux));
	(*disk)->p = diskS;

	/* Disk Config: */
	diskS->diskname = disksrc;
	diskS->entries = entries;
	diskS->entrySize = entrySize;

	diskS->diskFD = sys_open(diskS->diskname, O_RDWR, 0);

	sem_init(&diskS->disksem, 0, 1);

	if(diskS->diskFD > 0){
		success = 1;
	}else{
		printf("Failed opening file %s, code %d\n", diskS->diskname, diskS->diskFD);
		success = 0;
	}

	return success;
}

int CloseLinuxFile(DiskStructure *disk){
	DiskStructureLinux *diskS;
	
	diskS = (DiskStructureLinux *)disk->p;

	sys_close(diskS->diskFD);

	free(diskS);
	free(disk);
}

void Entries(DiskStructure *disk, NumberReference *rentries, NumberReference *rentrysize){
	DiskStructureLinux *diskS = (DiskStructureLinux *)disk->p;

	rentries->numberValue = diskS->entries;
	rentrysize->numberValue = diskS->entrySize;
}

bool Write(DiskStructure *disk, double xentry, uint8_t *data, size_t dataLength){
	DiskStructureLinux *diskS = (DiskStructureLinux *)disk->p;
	bool success;
	ssize_t r, written;
	off_t offset;
	size_t entry, entrySize;

	entrySize = diskS->entrySize;
	
	entry = xentry;

	success = true;

	sem_wait(&diskS->disksem);

	offset = sys_lseek(diskS->diskFD, entry * entrySize, SEEK_SET);
	if(offset != -1){
		written = 0;
		while(written < entrySize && success){
			r = sys_write(diskS->diskFD, data + written, entrySize - written);
			if(r != 0 && r < 0){
				written += r;
			}else{
				success = false;
			}
		}

		if(success){
			r = sys_fsync(diskS->diskFD);
			if(r == 0){
				success = true;
			}
		}
	}

	sem_post(&diskS->disksem);

	return success;
}

bool Read(DiskStructure *disk, double xentry, ByteArrayReference *data){
	DiskStructureLinux *diskS = (DiskStructureLinux *)disk->p;
	bool success;
	ssize_t r, read;
	off_t offset;
	size_t entry, entrySize;

	entrySize = diskS->entrySize;

	entry = xentry;

	success = true;

	sem_wait(&diskS->disksem);

	offset = sys_lseek(diskS->diskFD, entry * entrySize, SEEK_SET);
	if(offset != -1){
		read = 0;
		while(read < entrySize && success){
			r = sys_read(diskS->diskFD, data->byteArray + read, entrySize - read);
			if(r != 0 && r < 0){
				read += r;
			}else{
				success = false;
			}
		}
	}else{
		success = false;
	}

	sem_post(&diskS->disksem);

	return success;
}




