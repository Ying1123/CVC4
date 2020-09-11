/*********************                                                        */
/*! \file proof_node_updater.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Morgan Deters, Andrew Reynolds, Tim King
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2020 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Implementation of a utility for updating proof nodes
 **/

#include "expr/proof_node_updater.h"

#include "expr/lazy_proof.h"
#include "expr/proof_node_algorithm.h"

namespace CVC4 {

ProofNodeUpdaterCallback::ProofNodeUpdaterCallback() {}
ProofNodeUpdaterCallback::~ProofNodeUpdaterCallback() {}

bool ProofNodeUpdaterCallback::update(Node res,
                                      PfRule id,
                                      const std::vector<Node>& children,
                                      const std::vector<Node>& args,
                                      CDProof* cdp)
{
  return false;
}

ProofNodeUpdater::ProofNodeUpdater(ProofNodeManager* pnm,
                                   ProofNodeUpdaterCallback& cb)
    : d_pnm(pnm), d_cb(cb), d_debugFreeAssumps(false)
{
}

void ProofNodeUpdater::process(std::shared_ptr<ProofNode> pf)
{
  if (d_debugFreeAssumps)
  {
    if (Trace.isOn("pfnu-debug"))
    {
      Trace("pfnu-debug2") << "Initial proof: " << *pf.get() << std::endl;
      Trace("pfnu-debug")  << "ProofNodeUpdater::process" << std::endl;
      Trace("pfnu-debug") << "Expected free assumptions: " << std::endl;
      for (const Node& fa : d_freeAssumps)
      {
        Trace("pfnu-debug") << "- " << fa << std::endl;
      }
      std::vector<Node> assump;
      expr::getFreeAssumptions(pf.get(), assump);
      Trace("pfnu-debug") << "Current free assumptions: " << std::endl;
      for (const Node& fa : assump)
      {
        Trace("pfnu-debug") << "- " << fa << std::endl;
      }
      
    }
  }
  processInternal(pf, d_freeAssumps);
}

void ProofNodeUpdater::processInternal(std::shared_ptr<ProofNode> pf, const std::vector<Node>& fa)
{
  Trace("pf-process") << "ProofNodeUpdater::process" << std::endl;
  std::unordered_map<ProofNode*, bool> visited;
  std::unordered_map<ProofNode*, bool>::iterator it;
  std::vector<ProofNode*> visit;
  ProofNode* cur;
  visit.push_back(pf.get());
  std::map<Node, ProofNode*>::iterator itc;
  // NOTE: temporary, debugging
  unsigned counterReuse = 0;
  unsigned counterNew = 0;
  // A cache from formulas to proof nodes that are in the current scope.
  // Notice that we make a fresh recursive call to process if the current
  // rule is SCOPE below.
  std::map<Node, ProofNode*> resCache;
  TNode res;
  do
  {
    cur = visit.back();
    visit.pop_back();
    it = visited.find(cur);
    res = cur->getResult();
    if (it == visited.end())
    {
      itc = resCache.find(res);
      if (itc != resCache.end())
      {
        counterReuse++;
        // already have a proof
        visited[cur] = true;
        d_pnm->updateNode(cur, itc->second);
      }
      else
      {
        counterNew++;
        visited[cur] = false;
        // run update first
        runUpdate(cur, fa);
        visit.push_back(cur);
        const std::vector<std::shared_ptr<ProofNode>>& ccp = cur->getChildren();
        // now, process children
        for (const std::shared_ptr<ProofNode>& cp : ccp)
        {
          if (cp->getRule() == PfRule::SCOPE)
          {
            std::vector<Node> nfa;
            // if we are debugging free assumptions, update the set
            if (d_debugFreeAssumps)
            {
              nfa.insert(nfa.end(), fa.begin(), fa.end());
              const std::vector<Node>& args = cp->getArguments();
              nfa.insert(nfa.end(), args.begin(), args.end());
              Trace("pfnu-debug2") << "Process new scope with " << args << std::endl;
            }
            // Process in new call separately, since we should not cache
            // the results of proofs that have a different scope.
            processInternal(cp, nfa);
              Trace("pfnu-debug2") << "...finished process new scope" << std::endl;
            continue;
          }
          visit.push_back(cp.get());
        }
      }
      Trace("pf-process-debug")
          << "Processing " << counterReuse << "/" << counterNew << std::endl;
    }
    else if (!it->second)
    {
      visited[cur] = true;
      // cache result
      resCache[res] = cur;
    }
  } while (!visit.empty());
  Trace("pf-process") << "ProofNodeUpdater::process: finished" << std::endl;
}

void ProofNodeUpdater::runUpdate(ProofNode* cur, const std::vector<Node>& fa)
{
  // should it be updated?
  if (!d_cb.shouldUpdate(cur))
  {
    return;
  }
  PfRule id = cur->getRule();
  // use CDProof to open a scope for which the callback updates
  CDProof cpf(d_pnm);
  const std::vector<std::shared_ptr<ProofNode>>& cc = cur->getChildren();
  std::vector<Node> ccn;
  for (const std::shared_ptr<ProofNode>& cp : cc)
  {
    Node cpres = cp->getResult();
    ccn.push_back(cpres);
    // store in the proof
    cpf.addProof(cp);
  }
  Trace("pf-process-debug")
      << "Updating (" << cur->getRule() << ")..." << std::endl;
  Node res = cur->getResult();
  std::stringstream sorig;
  sorig << *cur;
  // only if the callback updated the node
  if (d_cb.update(res, id, ccn, cur->getArguments(), &cpf))
  {
    std::shared_ptr<ProofNode> npn = cpf.getProofFor(res);
    // then, update the original proof node based on this one
    Trace("pf-process-debug") << "Update node..." << std::endl;
    d_pnm->updateNode(cur, npn.get());
    Trace("pf-process-debug") << "...update node finished." << std::endl;
    if (d_debugFreeAssumps)
    {
      // We have that npn is a node we occurring the final updated version of
      // the proof. We can now debug based on the expected set of free
      // assumptions.
      Trace("pfnu-debug") << "Ensure update closed..." << std::endl;
      Trace("pfnu-debug") << "Original proof : " << sorig.str() << std::endl;
      pfnEnsureClosedWrt(npn.get(), fa, "pfnu-debug", "ProofNodeUpdater:postupdate");
    }
  }
  Trace("pf-process-debug") << "..finished" << std::endl;
}

void ProofNodeUpdater::setDebugFreeAssumptions(const std::vector<Node>& freeAssumps)
{
  d_freeAssumps.clear();
  d_freeAssumps.insert(d_freeAssumps.end(), freeAssumps.begin(), freeAssumps.end());
  d_debugFreeAssumps = true;
}

}  // namespace CVC4
