CONFIG -= qt

TEMPLATE = lib
DEFINES += CRM_001_LIBRARY

CONFIG += c++11 c99

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
	GDSPROJECT

QMAKE_CFLAGS	+= -Wno-unused -Wno-unused-parameter
QMAKE_CXXFLAGS	+= -Wno-unused -Wno-unused-parameter

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH	+= . \
	SYS \
	cgen \

SOURCES += \
	Backup.c \
	bnso.c \
	cgen/MCQ_dev.c \
	cgen/RCColor.c \
	cgen/RCText.c \
	cgen/TRMCM_var.c \
	cgen/TRModbusSlave_msg.c \
	cgen/TRQubusMaster_msg.c \
	cgen/__GS_driverapi.c \
	cgen/gdsPrjUserCInfo_CM.c \
	cgen/gdsPrjUserCInfo_VR.c \
	commands.c \
	control.c \
	crm.c \
	ctrl_abc_attach.c \
	ctrl_brush.c \
	ctrl_front_attach.c \
	ctrl_inter_attach.c \
	ctrl_light.c \
	ctrl_manipulator.c \
	ctrl_plough.c \
	ctrl_rear_attach.c \
	ctrl_side_attach.c \
	ctrl_top.c \
	ctrl_top_adrive.c \
	ctrl_top_attachment.c \
	ctrl_top_attachment_cr.c \
	ctrl_top_conveyor.c \
	drawfunctions.c \
	ftp.c \
	gs_easy_config.c \
	gs_easy_config_menu.c \
	gse_msgbox.c \
	gsecanopen.c \
	gsecondir.c \
	gseconsole.c \
	gsedebug.c \
	gseerrorlist.c \
	gseeth_functions.c \
	gseios.c \
	gselist.c \
	gsemaint.c \
	gsemaint_lifetime.c \
	gsemaint_runtime.c \
	gsemcm.c \
	gsescrolllist.c \
	hourcounter.c \
	hydvalvectrl.c \
	io.c \
	io_joystick_v85_b25.c \
	io_types.c \
	j1939.c \
	okb600.c \
	param.c \
	pinout.c \
	profile.c \
	response_error.c \
	sql_table.c \
	test_dbg.c \
	test_log.c \
	userj1939.c \
	usr_tools.c \
	visu.c \
	visu_ctrl.c \
	visu_error.c \
	visu_instrument.c \
	visu_io.c \
	visu_learning.c \
	visu_main.c \
	visu_material.c \
	visu_mcm_config.c \
	visu_menu.c \
	visu_param.c \
	visu_pinout.c \
	visu_profile.c \
	visu_properties.c \
	visu_reference.c \
	visu_sensors.c \
	visu_spreadsettings.c \
	visu_start.c \
	visu_statistik.c

HEADERS += \ \
	SYS/UserCAPI.h \
	SYS/UserCEvents.h \
	SYS/gdsPrjUserCInfo.h \
	SYS/gsAnimation.h \
	SYS/gsAnimationTypes.h \
	SYS/gsAnsiEsc.h \
	SYS/gsBluetooth.h \
	SYS/gsCanOpenSlave.h \
	SYS/gsCanTgmTypes.h \
	SYS/gsDebug.h \
	SYS/gsDropdownList.h \
	SYS/gsDropdownListTypes.h \
	SYS/gsLkfCodes.h \
	SYS/gsMenu.h \
	SYS/gsMenuTypes.h \
	SYS/gsModem.h \
	SYS/gsMsgFiFo.h \
	SYS/gsOSQVersion.h \
	SYS/gsOpcUaServer.h \
	SYS/gsOpcUaTypes.h \
	SYS/gsPDF.h \
	SYS/gsPPP.h \
	SYS/gsPrint.h \
	SYS/gsQuBus.h \
	SYS/gsQuBusEasy.h \
	SYS/gsQuBusEasyTypes.h \
	SYS/gsQuBusTypes.h \
	SYS/gsRemoteAccess.h \
	SYS/gsSocket.h \
	SYS/gsSocketNtpClient.h \
	SYS/gsSocketSmtpClient.h \
	SYS/gsSocketTCPClient.h \
	SYS/gsSocketTCPServer.h \
	SYS/gsSocketUDPPeer.h \
	SYS/gsTableView.h \
	SYS/gsTableViewTypes.h \
	SYS/gsThread.h \
	SYS/gsToVisu.h \
	SYS/gsTrendWriter.h \
	SYS/gsTrendWriterTypes.h \
	SYS/gsUsbHid.h \
	SYS/gsUsbSer.h \
	SYS/mcm.h \
	SYS/sqlite3.h \
	backup.h \
	bnso.h \
	cgen/ATBezier.h \
	cgen/ATTrendWriter.h \
	cgen/OSVersion.h \
	cgen/RCColor.h \
	cgen/RCText.h \
	cgen/TRJ1939_msg.h \
	cgen/TRMCM_var.h \
	cgen/TRModbusSlave_msg.h \
	cgen/TRQubusMaster_msg.h \
	commands.h \
	control.h \
	ctrl_abc_attach.h \
	ctrl_brush.h \
	ctrl_front_attach.h \
	ctrl_inter_attach.h \
	ctrl_light.h \
	ctrl_manipulator.h \
	ctrl_plough.h \
	ctrl_rear_attach.h \
	ctrl_side_attach.h \
	ctrl_top.h \
	ctrl_top_adrive.h \
	ctrl_top_attachment.h \
	ctrl_top_attachment_cr.h \
	ctrl_top_conveyor.h \
	drawfunctions.h \
	errorlist.h \
	gs_easy_config.h \
	gs_easy_config_menu.h \
	gse_msgbox.h \
	gsecanopen.h \
	gsecondir.h \
	gseconsole.h \
	gsedebug.h \
	gseerrorlist.h \
	gseeth_functions.h \
	gseios.h \
	gselist.h \
	gsemaint.h \
	gsemaint_lifetime.h \
	gsemaint_runtime.h \
	gsemcm.h \
	gseobj.h \
	gsescrolllist.h \
	hourcounter.h \
	hydvalvectrl.h \
	iconindex.h \
	io.h \
	io_joystick_v85_b25.h \
	io_types.h \
	j1939.h \
	light.h \
	objtab.h \
	okb600.h \
	param.h \
	pinout.h \
	profile.h \
	response_error.h \
	sql_table.h \
	test_dbg.h \
	test_log.h \
	userj1939.h \
	usr_tools.h \
	vartab.h \
	visu.h \
	visu_ctrl.h \
	visu_error.h \
	visu_instrument.h \
	visu_io.h \
	visu_learning.h \
	visu_main.h \
	visu_material.h \
	visu_mcm_config.h \
	visu_menu.h \
	visu_param.h \
	visu_pinout.h \
	visu_profile.h \
	visu_properties.h \
	visu_reference.h \
	visu_sensors.h \
	visu_spreadsettings.h \
	visu_start.h \
	visu_statistik.h


# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	GSe-VISU-W.ini \
	makefile
