; EXPECT: unsat
(set-logic ALL)
(set-info :status unsat)
(declare-datatypes ((Formula!953 0)) (((And!954 (lhs!955 Formula!953) (rhs!956 Formula!953)) (Not!957 (f!958 Formula!953)) (Or!959 (lhs!960 Formula!953) (rhs!961 Formula!953)) (Variable!962 (id!963 (_ BitVec 32))))))
(declare-fun error_value!964 () Bool)
(declare-fun isNNF!208 (Formula!953) Bool)
(declare-fun error_value!965 () Formula!953)
(declare-fun nnf!206 (Formula!953) Formula!953)
(declare-fun error_value!966 () Formula!953)
(assert (forall ((f!207 Formula!953)) (= (isNNF!208 f!207) (ite ((_ is And!954) f!207) (and (and (isNNF!208 (lhs!955 f!207)) (isNNF!208 (lhs!955 f!207))) (isNNF!208 (rhs!956 f!207))) (ite ((_ is Or!959) f!207) (and (and (isNNF!208 (lhs!960 f!207)) (isNNF!208 (lhs!960 f!207))) (isNNF!208 (rhs!961 f!207))) (ite ((_ is Not!957) f!207) false (ite ((_ is Variable!962) f!207) true error_value!964))))) ))
(assert (forall ((formula!205 Formula!953)) (= (nnf!206 formula!205) (ite ((_ is And!954) formula!205) (And!954 (nnf!206 (lhs!955 formula!205)) (nnf!206 (rhs!956 formula!205))) (ite ((_ is Or!959) formula!205) (Or!959 (nnf!206 (lhs!960 formula!205)) (nnf!206 (rhs!961 formula!205))) (ite (and (and ((_ is Not!957) formula!205) ((_ is Not!957) formula!205)) ((_ is And!954) (f!958 formula!205))) (Or!959 (nnf!206 (Not!957 (lhs!955 (f!958 formula!205)))) (nnf!206 (Not!957 (rhs!956 (f!958 formula!205))))) (ite (and (and ((_ is Not!957) formula!205) ((_ is Not!957) formula!205)) ((_ is Or!959) (f!958 formula!205))) (And!954 (nnf!206 (Not!957 (lhs!960 (f!958 formula!205)))) (nnf!206 (Not!957 (rhs!961 (f!958 formula!205))))) (ite (and (and ((_ is Not!957) formula!205) ((_ is Not!957) formula!205)) ((_ is Not!957) (f!958 formula!205))) (nnf!206 (f!958 (f!958 formula!205))) (ite ((_ is Not!957) formula!205) formula!205 (ite ((_ is Variable!962) formula!205) formula!205 error_value!965)))))))) ))
(assert (exists ((formula!205 Formula!953)) (not (=> ((_ is Variable!962) formula!205) (isNNF!208 (ite ((_ is And!954) formula!205) (And!954 (nnf!206 (lhs!955 formula!205)) (nnf!206 (rhs!956 formula!205))) (ite ((_ is Or!959) formula!205) (Or!959 (nnf!206 (lhs!960 formula!205)) (nnf!206 (rhs!961 formula!205))) (ite (and (and ((_ is Not!957) formula!205) ((_ is Not!957) formula!205)) ((_ is And!954) (f!958 formula!205))) (Or!959 (nnf!206 (Not!957 (lhs!955 (f!958 formula!205)))) (nnf!206 (Not!957 (rhs!956 (f!958 formula!205))))) (ite (and (and ((_ is Not!957) formula!205) ((_ is Not!957) formula!205)) ((_ is Or!959) (f!958 formula!205))) (And!954 (nnf!206 (Not!957 (lhs!960 (f!958 formula!205)))) (nnf!206 (Not!957 (rhs!961 (f!958 formula!205))))) (ite (and (and ((_ is Not!957) formula!205) ((_ is Not!957) formula!205)) ((_ is Not!957) (f!958 formula!205))) (nnf!206 (f!958 (f!958 formula!205))) (ite ((_ is Not!957) formula!205) formula!205 (ite ((_ is Variable!962) formula!205) formula!205 error_value!966)))))))))) ))
(check-sat)
