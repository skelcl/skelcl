SkelCL is a multi-GPU skeleton library based on OpenCL. It is actively
developed at the University of Münster, Germany, by Michel Steuwer and
others. For comments or questions feel free to contact the author:
michel.steuwer@uni-muenster.de

Most of the functionality is explained in detail in the following
peer-reviewed publications:

 - M. Steuwer, M. Friese, S. Albers, and S. Gorlatch:
   Introducing and Implementing the Allpairs Skeleton for Programming Multi-GPU Systems
   in International Journal of Parallel Programming, 2013, Springer

 - M. Steuwer and S. Gorlatch:
   High-Level Programming for Medical Imaging on Multi-GPU Systems using the SkelCL Library
   in Procedia Computer Science, Volumne 18, Pages 749-758, 2013, Elsevier

 - M. Steuwer and S. Gorlatch:
   SkelCL: Enhancing OpenCL for High-Level Programming of Multi-GPU Systems
   in Parallel Computing Technologies (PaCT), 2013, St. Petersburg, Russia

 - P. Kegel, M. Steuwer, and S. Gorlatch:
   Uniform High-Level Programming of Many-Core and Multi-GPU Systems
   in Transition of HPC Towards Exascale Computing, 2013, IOS Press

 - M. Steuwer, P. Kegel, and S. Gorlatch Sergei:
   A High-Level Programming Approach for Distributed Systems with Accelerators
   in New Trends in Software Methodologies, Tools and Techniques - Proceedings
   of the Eleventh SoMeT '12, 2012, Genoa, Italy

 - M. Steuwer, S. Gorlatch, S. Buß, and S. Breuer:
   Using the SkelCL Library for High-Level GPU Programming of 2D Applications
   in Euro-Par 2012: Parallel Processing Workshops, 2012, Rhodes Island,
   Greece

 - M. Steuwer, P. Kegel, and S. Gorlatch:
   Towards High-Level Programming of Multi-GPU Systems Using the SkelCL
   Library in 2012 IEEE International Symposium on Parallel and Distributed
   Processing Workshops and Phd Forum (IPDPSW), 2012, Shanghai, China

 - C. Kessler, S. Gorlatch, J. Emmyren, U. Dastgeer, M. Steuwer and
   P. Kegel:
   Skeleton Programming for Portable Many-Core Computing
   in Programming Multi-core and Many-core Computing Systems, Wiley
   (to appear)

 - M. Steuwer, P. Kegel, and S. Gorlatch:
   SkelCL - A Portable Skeleton Library for High-Level GPU Programming
   in 2011 IEEE International Symposium on Parallel and Distributed
   Processing Workshops and Phd Forum (IPDPSW), 2011, Anchorage, US

Two key abstractions are made in this library: algorithmic skeletons
and a unified memory abstraction. This also reflects in the classes
described here. The Skeleton class and subclasses describe
calculations performed on a GPU.
The Container classes, like Vector, provides a unified memory management
for CPU and GPU memory.

For more information visit <http://skelcl.uni-muenster.de> or contact the
main developer: michel.steuwer@uni-muenster.de

