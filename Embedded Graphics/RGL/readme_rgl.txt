=====================================================
Read Me File for RENESAS GRAPHICS LIBRARY for RH850/D1M
=====================================================
Updated:    28-Dec-2021 [US] - Remove restrictions of the DRW2D driver.  

========
Contents
========

  1. General Information 
  2. Release History
  3. Latest Changes
  4. Known Limitations
  5. Additional Info


======================
1. General Information 
======================

1.1 The RGL for D1Mx package is delivered as a Windows-Installer which extracts the RGL into the
following directory: rgl_ghs_D1Mx_[obj|src]_Vxxx, Vxxx is the current version of the library.
This directory contains this README.txt file and the RGL installer for one development environment.

    Note: Start menu entry is changed from last version. 
          If you want to clean up, please unistall the old version before this RGL is installed. 

1.2 To install the Renesas Graphic Library for D1Mx for the development environment of your choice
please use the following procedure:
    a. Use the installer to put the rgl_ghs_D1Mx_[obj|src]_Vxxx directory into a path that must not contain a "vlib" string.
    b. please note, that the path must not contain any whitespace character.

The library and the corresponding documentation can then be found in the given path. 
       
1.3 There are three demo programs inside the RGL which can be found in:
    rgl_ghs_D1Mx_[obj|src]_Vxxx/vlib/app/gfx_apps/simple_draw
    rgl_ghs_D1Mx_[obj|src]_Vxxx/vlib/app/gfx_apps/simple_capture
    rgl_ghs_D1Mx_[obj|src]_Vxxx/vlib/app/grape/grape_app
    rgl_ghs_D1Mx_[obj|src]_Vxxx/vlib/app/gfx_apps/simple_ovg (optional RGL package)

    simple_draw:
    - This application first decodes a JPEG file and then uses a Drw2D renderloop 
      to draw some primitives and textures.
      It also renders a sprite and a pattern that has been written to serial flash.
      On top the content is warped with the VideoOut Warping Engine.
      Detects the logo mark disappearing on DISCOM and VOCA.
      No O/S is used.
      In case of D1M1A and D1M1-V2, mango board has on board flash/RAM option.
      It is necessary to change the compile option to work with each flash/RAM option.
      Please see Makefile for the detail. (simple_draw/target/d1mx_mango_ghs/Makefile)
      
    simple_capture:
    - Connect a camera to VI1/CVBS and a QVGA display to CN40 (plus CSI when applicable).
      For D1M1(H),D1M1A,D1M1-V2 use a connection to CN44 instead.
      The HMI knob can be used to select on/off and pause/resume of the video input.   
      
    grape_app:
    - This application is GRAPE (application framework) based and offers an 
      interactive menu with several sample applications.
      The HMI knob can be used to select and exit the sample applications.
      Please consult the packaged Grape App user manual.
    
    simple_ovg (optional RGL package):
    - This application uses OpenVG 1.1 in combination with EGL to draw
      an animated cow with vector elements.
  

1.4 There are two possibilities to build and debug a demo program
    a. use cygwin and make (recommended)
    - cd rgl_ghs_D1Mx_[obj|src]_Vxxx/vlib/app/[gfx_apps|grape]/[simple_draw|simple_capture|grape_app|simple_ovg]/target/d1mx_mango_ghs
    - make 
    - start GHS Multi with the .out file
    b. use the GHS Multi project files
    - open rgl_ghs_D1Mx_[obj|src]_Vxxx/vlib/app/[gfx_apps|grape]/[simple_draw|simple_capture|grape_app|simple_ovg]/target/d1mx_mango_ghs/[simple_draw|simple_capture|grape_app|simple_ovg].gpj with multi.exe

1.5 Documentation
    Several documents are included in this package.
    a. RGL User Manual
        - doc/RGL-D1x_User-Manual.pdf        
        - doc/RGL-D1x_WM_User-Manual.pdf     
        - doc/RGL-D1x_SPEA_User-Manual.pdf   
        - doc/RGL-D1x_VDCE_User-Manual.pdf   
        - doc/RGL-D1x_VOWE_User-Manual.pdf   
        - doc/RGL-D1x_JCUA_User-Manual.pdf   
        - doc/RGL-D1x_SFMA_User-Manual.pdf   
        - doc/RGL-D1x_HYPB_User-Manual.pdf   
        - doc/RGL-D1x_OCTA_User-Manual.pdf   
        - doc/RGL-D1x_DRW2D_User-Manual.pdf  
        - doc/RGL-D1x_DISCOM_User-Manual.pdf 
        - doc/RGL-D1x_VOCA_User-Manual.pdf   
    b. Macro Referencea manual
        - doc/index.html   
        - this is the HTML documentation of the RGL API
        - this can be accessed in the Windows start menu entry
        - Other documents can be access from menu frame of this HTML documentation
    c. Porting layer guide
        - doc/RGL-D1x_Porting-Layer-Guide.pdf
    d. Grape_App User manual for the application grape_app
        - doc/Grape_app_UM.pdf
    e. this README.txt

==================
2. Release History
==================

E0.0.1  - 08-Mar-2013 [FZ] - 1st alpha release with Dave/HD rev0 driver V0.2
E0.1.0  - 07-Aug-2013 [FZ] - internal beta release with Dave/HD v8586 on SCIT v4
E0.2.0  - 09-Sep-2013 [FZ] - beta release with Dave/HD v8586 on SCIT v4
E0.3.0  - 17-Dec-2013 [FZ] - 2nd beta release with Dave/HD snapshot on SCIT v5
E0.4.0  - 15-Mar-2014 [FZ] - 3rd beta release with Dave/HD snapshot on SCIT v7 (arch like final D1M2)
E0.5.0  - 30-Jun-2014 [FZ] - final public beta release running on D1M2(H)
E0.5.1  - 25-Jul-2014 [FZ] - internal snapshot
E0.5.2  - 30-Jul-2014 [MS] - internal snapshot
E0.5.3  - 08-Aug-2014 [FZ] - public snapshot fixing Dave/HD, WM, SFMA and VDCE issues
E0.6.0  - 05-Sep-2014 [FZ] - public beta fixing Drw2D and WM issues
E0.6.1  - 10-Sep-2014 [FZ] - snapshot fixing Drw2D and WM issues
E0.6.2  - 12-Sep-2014 [FZ] - snapshot fixing WM issues
V1.0.0  - 30-Sep-2014 [FZ] - 1st validated release for public use
V1.1.0  - 31-Oct-2014 [FZ] - various issus fixed and verified
V1.1.1  - 18-Nov-2014 [FZ] - various issues fixed in SFMA and WM
V1.1.2  - 12-Dec-2014 [FZ] - Fix for Drw2D GpuFinished, improved doc, optimized memory footprint
V1.2.0  - 20-Mar-2015 [FZ] - maintenance release
V1.3.0  - 30-Jul-2015 [FZ] - maintenance release
V1.4.0  - 23-Oct-2015 [FZ] - maintenance release
V1.5.0  - 24-Mar-2016 [FZ] - maintenance release
V1.5.5  - 27-Jul-2016 [FZ] - maintenance release
V1.6.0  - 23-Sep-2016 [FZ] - maintenance release including some D1M1A alpha functionality
V1.7.0  - 17-Mar-2017 [FZ] - maintenance release including D1M1A beta functionality
V1.8.0  - 23-Jun-2017 [FZ] - maintenance release including D1M1A functionality
V1.9.0  - 31-Feb-2018 [FZ] - maintenance release including D1M1A and D1M1v2 functionality
V1.9.0a - 26-Jul-2018 [US] - Support GHS compiler 6.1.6 (2015-1-7)
V1.9.1  - 28-Sep-2018 [US] - Fix maximum value of active jobs for Drw2D[DHD].
V1.9.1a - 10-Oct-2018 [US] - Fix maximum value of active jobs for Drw2D[DHD].
V1.10.0a- 26-Jul-2019 [TS] - maintenance release including addition of safety design
V2.0.0a - 27-May-2020 [TS] - add DISCOM, VOCA driver and capture features. 
V2.1.0a - 28-Dec-2021 [US] - Remove restrictions of the DRW2D driver.

=================
3. Latest Changes
=================
3.1. Add new APIs. 
None.

3.2. Specification change
  - Example code DHD: R_UTIL_DHD_Config specificaton is changed.                   (OPC #81)
  - DRW2D: Add notes for Alpha blending.                                           (OPC #84)
  - DRW2D: Add detail specification of R_DRW2D_DrawQuadsUV.                        (OPC #85)
  - DRW2D: Clarify the specifications of the transformation matrix.                (OPC #86)
  - DRW2D: Add notes for color format.                                             (OPC #87)
  - DRW2D: Add notes for anti-alias.                                               (OPC #88)
  - DRW2D: Fix the blending formula of CONSTANT_ALPHA effect.                      (OPC #89)
  - DRW2D: Clarify the description of Pre-Multiplied Alpha blending.               (OPC #90)
  - DRW2D: Clarify the description of Texture Unit 1.                              (OPC #91)
  - DRW2D: Add notes for parameter TextureOffX and TextureOffY.                    (OPC #92)
  - DRW2D: Fix the input parameter range of Pitch size.                            (OPC #93)
  - DRW2D: Fix the blending formula of BlendMode=R_DRW2D_BLENDMODE_ADDITIVE.       (OPC #94)
  - DRW2D: Add notes for transform mode of Gradient effect.                        (OPC #95)
  - DRW2D: Clarify the description for R_DRW2D_CtxViewport.                        (OPC #96)

3.3. Bug fix
  - DRW2D: Fixed-Point Square Root issue.                                          (OPC #74)
  - DRW2D: Wrong output when drawing lines with Square CAP.                        (OPC #75)
  - DRW2D: The blending process for Anti-Aliasing may not be performed correctly.  (OPC #76)
  - DRW2D: DOT3 effect issue.                                                      (OPC #77)
  - DRW2D: Context selection issue.                                                (OPC #78)
  - DRW2D: Issue of Interpolate effect when using alpha channel only.              (OPC #79)
  - DRW2D: CLUT is not assigned as expected                                        (OPC #80)
  - DRW2D: The striping function is not disabled                                   (OPC #82)
  - DRW2D: Issue of Gradient effect control when using R_DRW2D_TRANSFORM_NONE      (OPC #83)


====================
4. Known Limitations
====================

4.1 Hardware dependecy
    Runs only with this hardware:
    - Device : D1M1(H) 1.2, D1M2(H) 3.0, D1M1A 1.1, D1M1v2 2.0
    - Board  : D1x Mango Board 1.0
    - Display: any VESA-compliant PC monitor or NEC QVGA Touch 240x320
    
4.2 RGL package 
    - Only supports GHS compiler environment version: 6.1.6 (2015-1-7)

==================
5. Additional Info
==================

5.1. Linker File
    - The included example linker file is: vlib/device/d1mx/src/rh850_ghs/d1mx.ld
     

============================================================
End of Read Me File for RENESAS GRAPHICS LIBRARY for RH850/D1M
============================================================
