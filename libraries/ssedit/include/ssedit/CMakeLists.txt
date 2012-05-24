# set header files
set (SSEDIT_PUBLIC_HEADERS
      CallExpression.h
      Cursor.h
      Delta.h
      DeltaTree.h
      Entity.h
      Function.h
      Parameter.h
      SourceFile.h
      SourceLocation.h
      SouceRange.h
      TempSourceFile.h
      Type.h
      Typedef.h
    )

# specify install into directory DESTINATION/include/ssedit
if (OS_LINUX)
  install (FILES ${SSEDIT_PUBLIC_HEADERS} DESTINATION include/ssedit)
endif (OS_LINUX)

