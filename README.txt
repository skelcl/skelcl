SkelCL is a multi-GPU skeleton library based on OpenCL. It is actively
developed at the University of Münster, Germany, by Michel Steuwer and
others. For comments or questions feel free to contact the author:
michel.steuwer@uni-muenster.de

Most of the functionality is explained in detail in the following
peer-reviewed publications:

 - M. Steuwer, P. Kegel, and S. Gorlatch:
   SkelCL - A Portable Skeleton Library for High-Level GPU Programming
   in 2011 IEEE International Symposium on Parallel and Distributed
   Processing Workshops and Phd Forum (IPDPSW), 2011, Anchorage, US

 - C. Kessler, S. Gorlatch, J. Emmyren, U. Dastgeer, M. Steuwer and
   P. Kegel:
   Skeleton Programming for Portable Many-Core Computing
   in Programming Multi-core and Many-core Computing Systems, Wiley
   (to appear)

 - M. Steuwer, P. Kegel, and S. Gorlatch:
   Towards High-Level Programming of Multi-GPU Systems Using the SkelCL
   Library in 2012 IEEE International Symposium on Parallel and Distributed
   Processing Workshops and Phd Forum (IPDPSW), 2012, Shanghai, China
   (to appear)

Two key abstractions are made in this library: algorithmic skeletons
and a unified memory abstraction. This also reflects in the classes
described here. The Skeleton class and subclasses describe
calculations performed on a GPU.
The Container classes, like Vector provides a unified memory management
for CPU and GPU memory.

For more information visit <http://skelcl.uni-muenster.de> or contact the
main developer: michel.steuwer@uni-muenster.de

