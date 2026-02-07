#! /bin/bash
rm -rf resources.tar.gz.tns
rm -rf body.tar.gz.tns
rm -rf parts.tar.gz.tns

tar -czf resources.tar.gz.tns resources
tar -czf body.tar.gz.tns body
tar -czf parts.tar.gz.tns parts
