(set-logic QF_SLIA)
(declare-fun a () (Seq (Seq Int)))
(declare-fun b () (Seq (Seq Int)))
(declare-fun c () (Seq (Seq Int)))
(declare-fun d () (Seq Int))
(declare-fun e () Int)
(declare-fun f () Int)
(assert (= (seq.extract d e 1) (seq.unit f)))
(assert (distinct a b (seq.++ (seq.unit d) c)))
(set-info :status sat)
(check-sat)
