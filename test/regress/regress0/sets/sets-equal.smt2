(set-logic ALL)
(set-info :status unsat)
(declare-fun x () Int)
(declare-fun y () Int)
(declare-fun z () Int)
(assert (= x y))
(declare-fun a () (Set Int))
(declare-fun b () (Set Int))
(assert (not (member x a)))
(assert (member y (union a b)))
(assert (= x z))
(assert (not (member z a)))
(assert (= a b))
(check-sat)
