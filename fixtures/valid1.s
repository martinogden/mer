Block
  [Stmt]
    Decl x = () _
    Asgn x = (ExpInt 3 _) _
    Return (ExpBinOp + (Ident x _) (ExpInt 4 _) _) _
