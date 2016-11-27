import os.path
import sys
import tarfile
import time
import zipfile
import StringIO

def read_file(path, use_crlf):
	with open(path, 'rb') as file:
		data = file.read()

	if '\0' not in data:
		data = data.replace('\r', '')
		if use_crlf:
			data = data.replace('\n', '\r\n')

	return data

def write_zip(target, arcprefix, sources):
	with zipfile.ZipFile(target, 'w', zipfile.ZIP_DEFLATED) as archive:
		for source in sorted(sources):
			data = read_file(source, use_crlf = True)
			path = os.path.join(arcprefix, source)
			archive.writestr(path, data)

def write_tar(target, arcprefix, sources, compression):
	with tarfile.open(target, 'w:' + compression) as archive:
		for source in sorted(sources):
			data = read_file(source, use_crlf = False)
			path = os.path.join(arcprefix, source)
			info = tarfile.TarInfo(path)
			info.size = len(data)
			info.mtime = time.time()
			archive.addfile(info, StringIO.StringIO(data))

if len(sys.argv) < 4:
	raise RuntimeError('Usage: python archive.py <target> <archive prefix> <source files>')

target = sys.argv[1]
arcprefix = sys.argv[2]
sources = sys.argv[3:]

if target.endswith('.zip'):
	write_zip(target, arcprefix, sources)
elif target.endswith('.tar.gz') or target.endswith('.tar.bz2'):
	write_tar(target, arcprefix, sources, compression = os.path.splitext(target)[1][1:])
else:
	raise NotImplementedError('File type not supported: ' + target)
