; EXPECT: unsat
(set-logic QF_NIA)
(declare-fun x () Int)
(assert (< x 0))
(assert (distinct (pow2 x) 0))
(check-sat)
