#!/usr/bin/env python3

# Initial work from https://github.com/esp8266/Arduino/blob/88be34e2dcb92023c36e2e63397aab9ebe6d987f/libraries/ESP8266WiFi/examples/BearSSL_CertStore/certs-from-mozilla.py
# Adapted with https://maakbaas.com/esp8266-iot-framework/logs/https-requests/

from __future__ import print_function
import csv
import os
import sys
from shutil import which

#from subprocess import Popen, PIPE, call, check_output
try:
    from urllib.request import urlopen
except Exception:
    from urllib2 import urlopen
try:
    from StringIO import StringIO
except Exception:
    from io import StringIO

# check if openssl is available
if which('openssl') is None and not os.path.isfile('./openssl') and not os.path.isfile('./openssl.exe'):
    raise Exception("You need to have openssl in PATH, installable from https://www.openssl.org/")

# Open FILE
CertificateStoreFile = open("spikes/lk_file_reception/CertificateStore.h", "w", encoding="utf8")

# FILE HEADER
CertificateStoreFile.write("// Leka - LekaOS" + "\n")
CertificateStoreFile.write("// Copyright 2022 APF France handicap" + "\n")
CertificateStoreFile.write("// SPDX-License-Identifier: Apache-2.0" + "\n\n")
CertificateStoreFile.write("#ifndef _LEKA_OS_CERTIFICATES_H_" + "\n")
CertificateStoreFile.write("#define _LEKA_OS_CERTIFICATES_H_" + "\n\n")
CertificateStoreFile.write("#include <lstd_array>" + "\n\n")
CertificateStoreFile.write("namespace leka {" + "\n\n")
CertificateStoreFile.write("constexpr auto CERTIFICATES = lstd::to_array<const char>(")

# Mozilla's URL for the CSV file with included PEM certs
mozurl = "https://ccadb-public.secure.force.com/"
mozurl += "mozilla/PublicAllIntermediateCertsWithPEMCSV"

response = urlopen(mozurl)
csvData = response.read()
if sys.version_info[0] > 2:
    csvData = csvData.decode('utf-8')
csvFile = StringIO(csvData)
csvReader = csv.reader(csvFile)

# Load the pems[] array from the URL
pems = []
containsSHA256WithRSA = None
containsExtKeyUsageServerAuth = None
containsExtKeyUsageClientAuth = None
for row in csvReader:
    containsSHA256WithRSA = False
    containsExtKeyUsageServerAuth = False
    containsExtKeyUsageClientAuth = False
    for item in row:
        containsSHA256WithRSA = containsSHA256WithRSA or (item.find("SHA256WithRSA") != -1)
        containsExtKeyUsageServerAuth = containsExtKeyUsageServerAuth or (item.find("ExtKeyUsageServerAuth") != -1)
        containsExtKeyUsageClientAuth = containsExtKeyUsageClientAuth or (item.find("ExtKeyUsageClientAuth") != -1)
        if item.startswith("'-----BEGIN CERTIFICATE-----") and containsSHA256WithRSA and containsExtKeyUsageServerAuth and containsExtKeyUsageClientAuth:
            pems.append(item)
del pems[0] # Remove header

# Format PEM certs in the file
for pem in pems:
    thisPem = pem.replace("'", "")
    thisPem = thisPem.replace("\r\n", "\n")
    thisPem = thisPem.replace("\n", "\\n\"\n\t\"")
    CertificateStoreFile.write("\n\t\"" + thisPem + "\\n\"")

# FILE FOOTER
CertificateStoreFile.write(");" + "\n\n")
CertificateStoreFile.write("}\t// namespace leka" + "\n\n")
CertificateStoreFile.write("#endif\t // _LEKA_OS_CERTIFICATES_H_" + "\n")

# Close FILE
CertificateStoreFile.close()
