rm -rf aclocal.m4 autom4te.cache stamp-h1 libtool configure config.* Makefile.in Makefile
rm -rf config/*

for a in utils flow_counter policer host_ifc ethl2 ethl3 acl fcf span sx_api net_lib sx_core sx_api_ib ibl3 ibl2 tcal3 sx_ib_lib; do
	rm -rf $a/Makefile 2>&1
	rm -rf $a/Makefile.in 2>&1
done
