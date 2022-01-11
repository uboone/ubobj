#ifndef NuSelectionBDT_h
#define NuSelectionBDT_h

#include <vector>

namespace nsm{

  class NuSelectionBDT{
  public:
  NuSelectionBDT();
  void reset();

  struct SPID{
    float shw_sp_num_mip_tracks;
    float shw_sp_num_muons;
    float shw_sp_num_pions;
    float shw_sp_num_protons;
    float shw_sp_proton_length_1;
    float shw_sp_proton_dqdx_1;
    float shw_sp_proton_energy_1;
    float shw_sp_proton_length_2;
    float shw_sp_proton_dqdx_2;
    float shw_sp_proton_energy_2;
    float shw_sp_n_good_showers;
    float shw_sp_n_20mev_showers;
    float shw_sp_n_br1_showers;
    float shw_sp_n_br2_showers;
    float shw_sp_n_br3_showers;
    float shw_sp_n_br4_showers;
    float shw_sp_n_20br1_showers;
    std::vector<int> *shw_sp_20mev_showers;
    std::vector<int> *shw_sp_br1_showers;
    std::vector<int> *shw_sp_br2_showers;
    std::vector<int> *shw_sp_br3_showers;
    std::vector<int> *shw_sp_br4_showers;
    float shw_sp_shw_vtx_dis;
    float shw_sp_max_shw_dis;
  };
  void SetSPID(SPID);
  const SPID & GetSPID() const;

  struct SPSHWID1{
	  float shw_sp_filled;
	  float shw_sp_flag;
	  float shw_sp_energy;
	  float shw_sp_vec_dQ_dx_0;
	  float shw_sp_vec_dQ_dx_1;
	  float shw_sp_max_dQ_dx_sample;
	  float shw_sp_n_below_threshold;
	  float shw_sp_n_below_zero;
	  float shw_sp_n_lowest;
	  float shw_sp_n_highest;
	  float shw_sp_lowest_dQ_dx;
	  float shw_sp_highest_dQ_dx;
	  float shw_sp_medium_dQ_dx;
	  float shw_sp_stem_length;
	  float shw_sp_length_main;
	  float shw_sp_length_total;
	  float shw_sp_angle_beam;
	  float shw_sp_iso_angle;
	  float shw_sp_n_vertex;
	  float shw_sp_n_good_tracks;
	  float shw_sp_E_indirect_max_energy;
	  float shw_sp_flag_all_above;
	  float shw_sp_min_dQ_dx_5;
	  float shw_sp_n_other_vertex;
	  float shw_sp_n_stem_size;
	  float shw_sp_flag_stem_trajectory;
	  float shw_sp_min_dis;
  };
  void SetSPSHWID1(SPSHWID1);
  const SPSHWID1 & GetSPSHWID1() const;

  struct SPSHWID2{
    float shw_sp_vec_median_dedx;
    float shw_sp_vec_mean_dedx;
	  float shw_sp_vec_dQ_dx_2;
	  float shw_sp_vec_dQ_dx_3;
	  float shw_sp_vec_dQ_dx_4;
	  float shw_sp_vec_dQ_dx_5;
	  float shw_sp_vec_dQ_dx_6;
	  float shw_sp_vec_dQ_dx_7;
	  float shw_sp_vec_dQ_dx_8;
	  float shw_sp_vec_dQ_dx_9;
	  float shw_sp_vec_dQ_dx_10;
	  float shw_sp_vec_dQ_dx_11;
	  float shw_sp_vec_dQ_dx_12;
	  float shw_sp_vec_dQ_dx_13;
	  float shw_sp_vec_dQ_dx_14;
	  float shw_sp_vec_dQ_dx_15;
	  float shw_sp_vec_dQ_dx_16;
	  float shw_sp_vec_dQ_dx_17;
	  float shw_sp_vec_dQ_dx_18;
	  float shw_sp_vec_dQ_dx_19;
  };
  void SetSPSHWID2(SPSHWID2);
  const SPSHWID2 & GetSPSHWID2() const;

  struct SPPi0Tagger1{
	  float shw_sp_pio_filled;
	  float shw_sp_pio_flag;
	  float shw_sp_pio_mip_id;
	  float shw_sp_pio_flag_pio;

	  float shw_sp_pio_1_flag;
	  float shw_sp_pio_1_mass;
	  float shw_sp_pio_1_pio_type;
	  float shw_sp_pio_1_energy_1;
	  float shw_sp_pio_1_energy_2;
	  float shw_sp_pio_1_dis_1;
	  float shw_sp_pio_1_dis_2;

	  std::vector<float> *shw_sp_pio_2_v_flag;
	  std::vector<float> *shw_sp_pio_2_v_dis2;
	  std::vector<float> *shw_sp_pio_2_v_angle2;
	  std::vector<float> *shw_sp_pio_2_v_acc_length;
  };
  void SetSPPi0Tagger1(SPPi0Tagger1);
  const SPPi0Tagger1 & GetSPPi0Tagger1() const;

  struct SPLowEMichel{
	  float shw_sp_lem_flag;
	  float shw_sp_lem_shower_total_length;
	  float shw_sp_lem_shower_main_length;
	  float shw_sp_lem_n_3seg;
	  float shw_sp_lem_e_charge;
	  float shw_sp_lem_e_dQdx;
	  float shw_sp_lem_shower_num_segs;
	  float shw_sp_lem_shower_num_main_segs;
  };
  void SetSPLowEMichel(SPLowEMichel);
  const SPLowEMichel & GetSPLowEMichel() const;

  struct SPBadReco1{
	  float shw_sp_br_filled;
	  float shw_sp_br1_flag;

	  float shw_sp_br1_1_flag;
	  float shw_sp_br1_1_shower_type;
	  float shw_sp_br1_1_vtx_n_segs;
	  float shw_sp_br1_1_energy;
	  float shw_sp_br1_1_n_segs;
	  float shw_sp_br1_1_flag_sg_topology;
	  float shw_sp_br1_1_flag_sg_trajectory;
	  float shw_sp_br1_1_sg_length;

	  float shw_sp_br1_2_flag;
	  float shw_sp_br1_2_energy;
	  float shw_sp_br1_2_n_connected;
	  float shw_sp_br1_2_max_length;
	  float shw_sp_br1_2_n_connected_1;
	  float shw_sp_br1_2_vtx_n_segs;
	  float shw_sp_br1_2_n_shower_segs;
	  float shw_sp_br1_2_max_length_ratio;
	  float shw_sp_br1_2_shower_length;

	  float shw_sp_br1_3_flag;
	  float shw_sp_br1_3_energy;
	  float shw_sp_br1_3_n_connected_p;
	  float shw_sp_br1_3_max_length_p;
	  float shw_sp_br1_3_n_shower_segs;
	  float shw_sp_br1_3_flag_sg_topology;
	  float shw_sp_br1_3_flag_sg_trajectory;
	  float shw_sp_br1_3_n_shower_main_segs;
	  float shw_sp_br1_3_sg_length;
  };
  void SetSPBadReco1(SPBadReco1);
  const SPBadReco1 & GetSPBadReco1() const;

  struct SPBadReco2{
	  float shw_sp_br_filled;

	  float shw_sp_br2_flag;
	  float shw_sp_br2_flag_single_shower;
	  float shw_sp_br2_num_valid_tracks;
	  float shw_sp_br2_energy;
	  float shw_sp_br2_angle1;
	  float shw_sp_br2_angle2;
	  float shw_sp_br2_angle;
	  float shw_sp_br2_angle3;
	  float shw_sp_br2_n_shower_main_segs;
	  float shw_sp_br2_max_angle;
	  float shw_sp_br2_sg_length;
	  float shw_sp_br2_flag_sg_trajectory;
  };
  void SetSPBadReco2(SPBadReco2);
  const SPBadReco2 & GetSPBadReco2() const;

  struct SPBadReco3{
	  float shw_sp_br_filled;
	  float shw_sp_br3_flag;

	  float shw_sp_br3_1_flag;
	  float shw_sp_br3_1_energy;
	  float shw_sp_br3_1_n_shower_segments;
	  float shw_sp_br3_1_sg_flag_trajectory;
	  float shw_sp_br3_1_sg_direct_length;
	  float shw_sp_br3_1_sg_length;
	  float shw_sp_br3_1_total_main_length;
	  float shw_sp_br3_1_total_length;
	  float shw_sp_br3_1_iso_angle;
	  float shw_sp_br3_1_sg_flag_topology;

	  float shw_sp_br3_2_flag;
	  float shw_sp_br3_2_n_ele;
	  float shw_sp_br3_2_n_other;
	  float shw_sp_br3_2_energy;
	  float shw_sp_br3_2_total_main_length;
	  float shw_sp_br3_2_total_length;
	  float shw_sp_br3_2_other_fid;

	  std::vector<float> *shw_sp_br3_3_v_flag;
	  std::vector<float> *shw_sp_br3_3_v_energy;
	  std::vector<float> *shw_sp_br3_3_v_angle;
	  std::vector<float> *shw_sp_br3_3_v_dir_length;
	  std::vector<float> *shw_sp_br3_3_v_length;

	  float shw_sp_br3_4_flag;
	  float shw_sp_br3_4_acc_length;
	  float shw_sp_br3_4_total_length;
	  float shw_sp_br3_4_energy;

	  std::vector<float> *shw_sp_br3_5_v_flag;
	  std::vector<float> *shw_sp_br3_5_v_dir_length;
	  std::vector<float> *shw_sp_br3_5_v_total_length;
	  std::vector<float> *shw_sp_br3_5_v_flag_avoid_muon_check;
	  std::vector<float> *shw_sp_br3_5_v_n_seg;
	  std::vector<float> *shw_sp_br3_5_v_angle;
	  std::vector<float> *shw_sp_br3_5_v_sg_length;
	  std::vector<float> *shw_sp_br3_5_v_energy;
	  std::vector<float> *shw_sp_br3_5_v_n_main_segs;
	  std::vector<float> *shw_sp_br3_5_v_n_segs;
	  std::vector<float> *shw_sp_br3_5_v_shower_main_length;
	  std::vector<float> *shw_sp_br3_5_v_shower_total_length;

	  std::vector<float> *shw_sp_br3_6_v_flag;
	  std::vector<float> *shw_sp_br3_6_v_angle;
	  std::vector<float> *shw_sp_br3_6_v_angle1;
	  std::vector<float> *shw_sp_br3_6_v_flag_shower_trajectory;
	  std::vector<float> *shw_sp_br3_6_v_direct_length;
	  std::vector<float> *shw_sp_br3_6_v_length;
	  std::vector<float> *shw_sp_br3_6_v_n_other_vtx_segs;
	  std::vector<float> *shw_sp_br3_6_v_energy;

	  float shw_sp_br3_7_flag;
	  float shw_sp_br3_7_energy;
	  float shw_sp_br3_7_min_angle;
	  float shw_sp_br3_7_sg_length;
	  float shw_sp_br3_7_shower_main_length;

	  float shw_sp_br3_8_flag;
	  float shw_sp_br3_8_max_dQ_dx;
	  float shw_sp_br3_8_energy;
	  float shw_sp_br3_8_n_main_segs;
	  float shw_sp_br3_8_shower_main_length;
	  float shw_sp_br3_8_shower_length;
  };
  void SetSPBadReco3(SPBadReco3);
  const SPBadReco3 & GetSPBadReco3() const;

  struct SPBadReco4{
	  float shw_sp_br_filled;
	  float shw_sp_br4_flag;

	  float shw_sp_br4_1_flag;
	  float shw_sp_br4_1_shower_main_length;
	  float shw_sp_br4_1_shower_total_length;
	  float shw_sp_br4_1_min_dis;
	  float shw_sp_br4_1_energy;
	  float shw_sp_br4_1_flag_avoid_muon_check;
	  float shw_sp_br4_1_n_vtx_segs;
	  float shw_sp_br4_1_n_main_segs;

	  float shw_sp_br4_2_flag;
	  float shw_sp_br4_2_ratio_45;
	  float shw_sp_br4_2_ratio_35;
	  float shw_sp_br4_2_ratio_25;
	  float shw_sp_br4_2_ratio_15;
	  float shw_sp_br4_2_energy;
	  float shw_sp_br4_2_ratio1_45;
	  float shw_sp_br4_2_ratio1_35;
	  float shw_sp_br4_2_ratio1_25;
	  float shw_sp_br4_2_ratio1_15;
	  float shw_sp_br4_2_iso_angle;
	  float shw_sp_br4_2_iso_angle1;
	  float shw_sp_br4_2_angle;
  };
  void SetSPBadReco4(SPBadReco4);
  const SPBadReco4 & GetSPBadReco4() const;

  struct SPHighEoverlap{
	  float shw_sp_hol_flag;

	  float shw_sp_hol_1_flag;
	  float shw_sp_hol_1_n_valid_tracks;
	  float shw_sp_hol_1_min_angle;
	  float shw_sp_hol_1_energy;
	  float shw_sp_hol_1_flag_all_shower;
	  float shw_sp_hol_1_min_length;

	  float shw_sp_hol_2_flag;
	  float shw_sp_hol_2_min_angle;
	  float shw_sp_hol_2_medium_dQ_dx;
	  float shw_sp_hol_2_ncount;
	  float shw_sp_hol_2_energy;
  };
  void SetSPHighEoverlap(SPHighEoverlap);
  const SPHighEoverlap & GetSPHighEoverlap() const;

  struct SPLowEoverlap{
	  float shw_sp_lol_flag;

	  std::vector<float> *shw_sp_lol_1_v_flag;
	  std::vector<float> *shw_sp_lol_1_v_energy;
	  std::vector<float> *shw_sp_lol_1_v_vtx_n_segs;
	  std::vector<float> *shw_sp_lol_1_v_nseg;
	  std::vector<float> *shw_sp_lol_1_v_angle;

	  std::vector<float> *shw_sp_lol_2_v_flag;
	  std::vector<float> *shw_sp_lol_2_v_length;
	  std::vector<float> *shw_sp_lol_2_v_angle;
	  std::vector<float> *shw_sp_lol_2_v_type;
	  std::vector<float> *shw_sp_lol_2_v_vtx_n_segs;
	  std::vector<float> *shw_sp_lol_2_v_energy;
	  std::vector<float> *shw_sp_lol_2_v_shower_main_length;
	  std::vector<float> *shw_sp_lol_2_v_flag_dir_weak;

	  float shw_sp_lol_3_flag;
	  float shw_sp_lol_3_angle_beam;
	  float shw_sp_lol_3_n_valid_tracks;
	  float shw_sp_lol_3_min_angle;
	  float shw_sp_lol_3_vtx_n_segs;
	  float shw_sp_lol_3_energy;
	  float shw_sp_lol_3_shower_main_length;
	  float shw_sp_lol_3_n_out;
	  float shw_sp_lol_3_n_sum;
  };
  void SetSPLowEoverlap(SPLowEoverlap);
  const SPLowEoverlap & GetSPLowEoverlap() const;

  struct CosmicTagger{
	  float cosmic_filled;
	  float cosmic_flag;
	  float cosmic_n_solid_tracks;
	  float cosmic_energy_main_showers;
	  float cosmic_energy_direct_showers;
	  float cosmic_energy_indirect_showers;
	  float cosmic_n_direct_showers;
	  float cosmic_n_indirect_showers;
	  float cosmic_n_main_showers;
  };
  void SetCosmicTagger(CosmicTagger);
  const CosmicTagger & GetCosmicTagger() const;

  struct GapID{
	  float gap_filled;
	  float gap_flag;
	  float gap_flag_prolong_u;
	  float gap_flag_prolong_v;
	  float gap_flag_prolong_w;
	  float gap_flag_parallel;
	  float gap_n_points;
	  float gap_n_bad;
	  float gap_energy;
	  float gap_num_valid_tracks;
	  float gap_flag_single_shower;
  };
  void SetGapID(GapID);
  const GapID & GetGapID() const;

  struct MipCheck{
	  float mip_quality_filled;
	  float mip_quality_flag;
	  float mip_quality_energy;
	  float mip_quality_overlap;
	  float mip_quality_n_showers;
	  float mip_quality_n_tracks;
	  float mip_quality_flag_inside_pi0;
	  float mip_quality_n_pi0_showers;
	  float mip_quality_shortest_length;
	  float mip_quality_acc_length;
	  float mip_quality_shortest_angle;
	  float mip_quality_flag_proton;
  };
  void SetMipCheck(MipCheck);
  const MipCheck & GetMipCheck() const;

  struct MipID1{
	  float mip_filled;
	  float mip_flag;
	  float mip_energy;
	  float mip_n_end_reduction;
	  float mip_n_first_mip;
	  float mip_n_first_non_mip;
	  float mip_n_first_non_mip_1;
	  float mip_n_first_non_mip_2;
	  float mip_vec_dQ_dx_0;
	  float mip_vec_dQ_dx_1;
	  float mip_max_dQ_dx_sample;
	  float mip_n_below_threshold;
	  float mip_n_below_zero;
	  float mip_n_lowest;
	  float mip_n_highest;
	  float mip_lowest_dQ_dx;
	  float mip_highest_dQ_dx;
	  float mip_medium_dQ_dx;
	  float mip_stem_length;
	  float mip_length_main;
	  float mip_length_total;
	  float mip_angle_beam;
	  float mip_iso_angle;
	  float mip_n_vertex;
	  float mip_n_good_tracks;
	  float mip_E_indirect_max_energy;
	  float mip_flag_all_above;
	  float mip_min_dQ_dx_5;
	  float mip_n_other_vertex;
	  float mip_n_stem_size;
	  float mip_flag_stem_trajectory;
	  float mip_min_dis;
  };
  void SetMipID1(MipID1);
  const MipID1 & GetMipID1() const;

  struct MipID2{
	  float mip_vec_dQ_dx_2;
	  float mip_vec_dQ_dx_3;
	  float mip_vec_dQ_dx_4;
	  float mip_vec_dQ_dx_5;
	  float mip_vec_dQ_dx_6;
	  float mip_vec_dQ_dx_7;
	  float mip_vec_dQ_dx_8;
	  float mip_vec_dQ_dx_9;
	  float mip_vec_dQ_dx_10;
	  float mip_vec_dQ_dx_11;
	  float mip_vec_dQ_dx_12;
	  float mip_vec_dQ_dx_13;
	  float mip_vec_dQ_dx_14;
	  float mip_vec_dQ_dx_15;
	  float mip_vec_dQ_dx_16;
	  float mip_vec_dQ_dx_17;
	  float mip_vec_dQ_dx_18;
	  float mip_vec_dQ_dx_19;
  };
  void SetMipID2(MipID2);
  const MipID2 & GetMipID2() const;

  struct Pi0Tagger1{
	  float pio_filled;
	  float pio_flag;
	  float pio_mip_id;
	  float pio_flag_pio;

	  float pio_1_flag;
	  float pio_1_mass;
	  float pio_1_pio_type;
	  float pio_1_energy_1;
	  float pio_1_energy_2;
	  float pio_1_dis_1;
	  float pio_1_dis_2;

	  std::vector<float> *pio_2_v_flag;
	  std::vector<float> *pio_2_v_dis2;
	  std::vector<float> *pio_2_v_angle2;
	  std::vector<float> *pio_2_v_acc_length;
  };
  void SetPi0Tagger1(Pi0Tagger1);
  const Pi0Tagger1 & GetPi0Tagger1() const;

  struct Pi0Tagger2{
	  float sig_flag;

	  std::vector<float> *sig_1_v_flag;
	  std::vector<float> *sig_1_v_angle;
	  std::vector<float> *sig_1_v_flag_single_shower;
	  std::vector<float> *sig_1_v_energy;
	  std::vector<float> *sig_1_v_energy_1;

	  std::vector<float> *sig_2_v_flag;
	  std::vector<float> *sig_2_v_energy;
	  std::vector<float> *sig_2_v_shower_angle;
	  std::vector<float> *sig_2_v_flag_single_shower;
	  std::vector<float> *sig_2_v_medium_dQ_dx;
	  std::vector<float> *sig_2_v_start_dQ_dx;
  };
  void SetPi0Tagger2(Pi0Tagger2);
  const Pi0Tagger2 & GetPi0Tagger2() const;

  struct MultiGamma1{
	  float mgo_flag;
	  float mgo_energy;
	  float mgo_max_energy;
	  float mgo_total_energy;
	  float mgo_n_showers;
	  float mgo_max_energy_1;
	  float mgo_max_energy_2;
	  float mgo_total_other_energy;
	  float mgo_n_total_showers;
	  float mgo_total_other_energy_1;
  };
  void SetMultiGamma1(MultiGamma1);
  const MultiGamma1 & GetMultiGamma1() const;

  struct MultiGamma2{
	  float mgt_flag;
	  float mgt_flag_single_shower;
	  float mgt_max_energy;
	  float mgt_energy;
	  float mgt_total_other_energy;
	  float mgt_max_energy_1;
	  float mgt_e_indirect_max_energy;
	  float mgt_e_direct_max_energy;
	  float mgt_n_direct_showers;
	  float mgt_e_direct_total_energy;
	  float mgt_flag_indirect_max_pio;
	  float mgt_e_indirect_total_energy;
  };
  void SetMultiGamma2(MultiGamma2);
  const MultiGamma2 & GetMultiGamma2() const;

  struct SingleGamma1{
	  float stw_flag;

	  float stw_1_flag;
	  float stw_1_energy;
	  float stw_1_dis;
	  float stw_1_dQ_dx;
	  float stw_1_flag_single_shower;
	  float stw_1_n_pi0;
	  float stw_1_num_valid_tracks;

	  std::vector<float> *stw_2_v_flag;
	  std::vector<float> *stw_2_v_medium_dQ_dx;
	  std::vector<float> *stw_2_v_energy;
	  std::vector<float> *stw_2_v_angle;
	  std::vector<float> *stw_2_v_dir_length;
	  std::vector<float> *stw_2_v_max_dQ_dx;

	  std::vector<float> *stw_3_v_flag;
	  std::vector<float> *stw_3_v_angle;
	  std::vector<float> *stw_3_v_dir_length;
	  std::vector<float> *stw_3_v_energy;
	  std::vector<float> *stw_3_v_medium_dQ_dx;

	  std::vector<float> *stw_4_v_flag;
	  std::vector<float> *stw_4_v_angle;
	  std::vector<float> *stw_4_v_dis;
	  std::vector<float> *stw_4_v_energy;
  };
  void SetSingleGamma1(SingleGamma1);
  const SingleGamma1 & GetSingleGamma1() const;

  struct SingleGamma2{
	  float spt_flag;
	  float spt_flag_single_shower;
	  float spt_energy;
	  float spt_shower_main_length;
	  float spt_shower_total_length;
	  float spt_angle_beam;
	  float spt_angle_vertical;
	  float spt_max_dQ_dx;
	  float spt_angle_beam_1;
	  float spt_angle_drift;
	  float spt_angle_drift_1;
	  float spt_num_valid_tracks;
	  float spt_n_vtx_segs;
	  float spt_max_length;
  };
  void SetSingleGamma2(SingleGamma2);
  const SingleGamma2 & GetSingleGamma2() const;

  struct StemLen{
	  float stem_len_flag;
	  float stem_len_energy;
	  float stem_len_length;
	  float stem_len_flag_avoid_muon_check;
	  float stem_len_num_daughters;
	  float stem_len_daughter_length;
  };
  void SetStemLen(StemLen);
  const StemLen & GetStemLen() const;

  struct LowEMichel{
	  float lem_flag;
	  float lem_shower_total_length;
	  float lem_shower_main_length;
	  float lem_n_3seg;
	  float lem_e_charge;
	  float lem_e_dQdx;
	  float lem_shower_num_segs;
	  float lem_shower_num_main_segs;
  };
  void SetLowEMichel(LowEMichel);
  const LowEMichel & GetLowEMichel() const;

  struct BrokenMuon{
	  float brm_flag;
	  float brm_n_mu_segs;
	  float brm_Ep;
	  float brm_energy;
	  float brm_acc_length;
	  float brm_shower_total_length;
	  float brm_connected_length;
	  float brm_n_size;
	  float brm_acc_direct_length;
	  float brm_n_shower_main_segs;
	  float brm_n_mu_main;
  };
  void SetBrokenMuon(BrokenMuon);
  const BrokenMuon & GetBrokenMuon() const;

  struct MuEnergy{
	  float cme_flag;
	  float cme_mu_energy;
	  float cme_energy;
	  float cme_mu_length;
	  float cme_length;
	  float cme_angle_beam;
  };
  void SetMuEnergy(MuEnergy);
  const MuEnergy & GetMuEnergy() const;

  struct ShowerAngle{
	  float anc_flag;
	  float anc_energy;
	  float anc_angle;
	  float anc_max_angle;
	  float anc_max_length;
	  float anc_acc_forward_length;
	  float anc_acc_backward_length;
	  float anc_acc_forward_length1;
	  float anc_shower_main_length;
	  float anc_shower_total_length;
	  float anc_flag_main_outside;
  };
  void SetShowerAngle(ShowerAngle);
  const ShowerAngle & GetShowerAngle() const;

  struct BadStem{
	  float stem_dir_filled;
	  float stem_dir_flag;
	  float stem_dir_flag_single_shower;
	  float stem_dir_angle;
	  float stem_dir_energy;
	  float stem_dir_angle1;
	  float stem_dir_angle2;
	  float stem_dir_angle3;
	  float stem_dir_ratio;
  };
  void SetBadStem(BadStem);
  const BadStem & GetBadStem() const;

  struct VtxInShw{
	  float vis_flag;

	  float vis_1_filled;
	  float vis_1_flag;
	  float vis_1_n_vtx_segs;
	  float vis_1_energy;
	  float vis_1_num_good_tracks;
	  float vis_1_max_angle;
	  float vis_1_max_shower_angle;
	  float vis_1_tmp_length1;
	  float vis_1_tmp_length2;
	  float vis_1_particle_type;

	  float vis_2_filled;
	  float vis_2_flag;
	  float vis_2_n_vtx_segs;
	  float vis_2_min_angle;
	  float vis_2_min_weak_track;
	  float vis_2_angle_beam;
	  float vis_2_min_angle1;
	  float vis_2_iso_angle1;
	  float vis_2_min_medium_dQ_dx;
	  float vis_2_min_length;
	  float vis_2_sg_length;
	  float vis_2_max_angle;
	  float vis_2_max_weak_track;
  };
  void SetVtxInShw(VtxInShw);
  const VtxInShw & GetVtxInShw() const;

  struct BadReco1{
	  float br_filled;
	  float br1_flag;

	  float br1_1_flag;
	  float br1_1_shower_type;
	  float br1_1_vtx_n_segs;
	  float br1_1_energy;
	  float br1_1_n_segs;
	  float br1_1_flag_sg_topology;
	  float br1_1_flag_sg_trajectory;
	  float br1_1_sg_length;

	  float br1_2_flag;
	  float br1_2_energy;
	  float br1_2_n_connected;
	  float br1_2_max_length;
	  float br1_2_n_connected_1;
	  float br1_2_vtx_n_segs;
	  float br1_2_n_shower_segs;
	  float br1_2_max_length_ratio;
	  float br1_2_shower_length;

	  float br1_3_flag;
	  float br1_3_energy;
	  float br1_3_n_connected_p;
	  float br1_3_max_length_p;
	  float br1_3_n_shower_segs;
	  float br1_3_flag_sg_topology;
	  float br1_3_flag_sg_trajectory;
	  float br1_3_n_shower_main_segs;
	  float br1_3_sg_length;
  };
  void SetBadReco1(BadReco1);
  const BadReco1 & GetBadReco1() const;

  struct BadReco2{
	  float br_filled;

	  float br2_flag;
	  float br2_flag_single_shower;
	  float br2_num_valid_tracks;
	  float br2_energy;
	  float br2_angle1;
	  float br2_angle2;
	  float br2_angle;
	  float br2_angle3;
	  float br2_n_shower_main_segs;
	  float br2_max_angle;
	  float br2_sg_length;
	  float br2_flag_sg_trajectory;
  };
  void SetBadReco2(BadReco2);
  const BadReco2 & GetBadReco2() const;

  struct BadReco3{
	  float br_filled;
	  float br3_flag;

	  float br3_1_flag;
	  float br3_1_energy;
	  float br3_1_n_shower_segments;
	  float br3_1_sg_flag_trajectory;
	  float br3_1_sg_direct_length;
	  float br3_1_sg_length;
	  float br3_1_total_main_length;
	  float br3_1_total_length;
	  float br3_1_iso_angle;
	  float br3_1_sg_flag_topology;

	  float br3_2_flag;
	  float br3_2_n_ele;
	  float br3_2_n_other;
	  float br3_2_energy;
	  float br3_2_total_main_length;
	  float br3_2_total_length;
	  float br3_2_other_fid;

	  std::vector<float> *br3_3_v_flag;
	  std::vector<float> *br3_3_v_energy;
	  std::vector<float> *br3_3_v_angle;
	  std::vector<float> *br3_3_v_dir_length;
	  std::vector<float> *br3_3_v_length;

	  float br3_4_flag;
	  float br3_4_acc_length;
	  float br3_4_total_length;
	  float br3_4_energy;

	  std::vector<float> *br3_5_v_flag;
	  std::vector<float> *br3_5_v_dir_length;
	  std::vector<float> *br3_5_v_total_length;
	  std::vector<float> *br3_5_v_flag_avoid_muon_check;
	  std::vector<float> *br3_5_v_n_seg;
	  std::vector<float> *br3_5_v_angle;
	  std::vector<float> *br3_5_v_sg_length;
	  std::vector<float> *br3_5_v_energy;
	  std::vector<float> *br3_5_v_n_main_segs;
	  std::vector<float> *br3_5_v_n_segs;
	  std::vector<float> *br3_5_v_shower_main_length;
	  std::vector<float> *br3_5_v_shower_total_length;

	  std::vector<float> *br3_6_v_flag;
	  std::vector<float> *br3_6_v_angle;
	  std::vector<float> *br3_6_v_angle1;
	  std::vector<float> *br3_6_v_flag_shower_trajectory;
	  std::vector<float> *br3_6_v_direct_length;
	  std::vector<float> *br3_6_v_length;
	  std::vector<float> *br3_6_v_n_other_vtx_segs;
	  std::vector<float> *br3_6_v_energy;

	  float br3_7_flag;
	  float br3_7_energy;
	  float br3_7_min_angle;
	  float br3_7_sg_length;
	  float br3_7_shower_main_length;

	  float br3_8_flag;
	  float br3_8_max_dQ_dx;
	  float br3_8_energy;
	  float br3_8_n_main_segs;
	  float br3_8_shower_main_length;
	  float br3_8_shower_length;
  };
  void SetBadReco3(BadReco3);
  const BadReco3 & GetBadReco3() const;

  struct BadReco4{
	  float br_filled;
	  float br4_flag;

	  float br4_1_flag;
	  float br4_1_shower_main_length;
	  float br4_1_shower_total_length;
	  float br4_1_min_dis;
	  float br4_1_energy;
	  float br4_1_flag_avoid_muon_check;
	  float br4_1_n_vtx_segs;
	  float br4_1_n_main_segs;

	  float br4_2_flag;
	  float br4_2_ratio_45;
	  float br4_2_ratio_35;
	  float br4_2_ratio_25;
	  float br4_2_ratio_15;
	  float br4_2_energy;
	  float br4_2_ratio1_45;
	  float br4_2_ratio1_35;
	  float br4_2_ratio1_25;
	  float br4_2_ratio1_15;
	  float br4_2_iso_angle;
	  float br4_2_iso_angle1;
	  float br4_2_angle;
  };
  void SetBadReco4(BadReco4);
  const BadReco4 & GetBadReco4() const;

  struct TrackOverCluster{
	  float tro_flag;

	  std::vector<float> *tro_1_v_flag;
	  std::vector<float> *tro_1_v_particle_type;
	  std::vector<float> *tro_1_v_flag_dir_weak;
	  std::vector<float> *tro_1_v_min_dis;
	  std::vector<float> *tro_1_v_sg1_length;
	  std::vector<float> *tro_1_v_shower_main_length;
	  std::vector<float> *tro_1_v_max_n_vtx_segs;
	  std::vector<float> *tro_1_v_tmp_length;
	  std::vector<float> *tro_1_v_medium_dQ_dx;
	  std::vector<float> *tro_1_v_dQ_dx_cut;
	  std::vector<float> *tro_1_v_flag_shower_topology;

	  std::vector<float> *tro_2_v_flag;
	  std::vector<float> *tro_2_v_energy;
	  std::vector<float> *tro_2_v_stem_length;
	  std::vector<float> *tro_2_v_iso_angle;
	  std::vector<float> *tro_2_v_max_length;
	  std::vector<float> *tro_2_v_angle;

	  float tro_3_flag;
	  float tro_3_stem_length;
	  float tro_3_n_muon_segs;
	  float tro_3_energy;

	  std::vector<float> *tro_4_v_flag;
	  std::vector<float> *tro_4_v_dir2_mag;
	  std::vector<float> *tro_4_v_angle;
	  std::vector<float> *tro_4_v_angle1;
	  std::vector<float> *tro_4_v_angle2;
	  std::vector<float> *tro_4_v_length;
	  std::vector<float> *tro_4_v_length1;
	  std::vector<float> *tro_4_v_medium_dQ_dx;
	  std::vector<float> *tro_4_v_end_dQ_dx;
	  std::vector<float> *tro_4_v_energy;
	  std::vector<float> *tro_4_v_shower_main_length;
	  std::vector<float> *tro_4_v_flag_shower_trajectory;

	  std::vector<float> *tro_5_v_flag;
	  std::vector<float> *tro_5_v_max_angle;
	  std::vector<float> *tro_5_v_min_angle;
	  std::vector<float> *tro_5_v_max_length;
	  std::vector<float> *tro_5_v_iso_angle;
	  std::vector<float> *tro_5_v_n_vtx_segs;
	  std::vector<float> *tro_5_v_min_count;
	  std::vector<float> *tro_5_v_max_count;
	  std::vector<float> *tro_5_v_energy;
  };
  void SetTrackOverCluster(TrackOverCluster);
  const TrackOverCluster & GetTrackOverCluster() const;

  struct HighEoverlap{
	  float hol_flag;

	  float hol_1_flag;
	  float hol_1_n_valid_tracks;
	  float hol_1_min_angle;
	  float hol_1_energy;
	  float hol_1_flag_all_shower;
	  float hol_1_min_length;

	  float hol_2_flag;
	  float hol_2_min_angle;
	  float hol_2_medium_dQ_dx;
	  float hol_2_ncount;
	  float hol_2_energy;
  };
  void SetHighEoverlap(HighEoverlap);
  const HighEoverlap & GetHighEoverlap() const;

  struct LowEoverlap{
	  float lol_flag;

	  std::vector<float> *lol_1_v_flag;
	  std::vector<float> *lol_1_v_energy;
	  std::vector<float> *lol_1_v_vtx_n_segs;
	  std::vector<float> *lol_1_v_nseg;
	  std::vector<float> *lol_1_v_angle;

	  std::vector<float> *lol_2_v_flag;
	  std::vector<float> *lol_2_v_length;
	  std::vector<float> *lol_2_v_angle;
	  std::vector<float> *lol_2_v_type;
	  std::vector<float> *lol_2_v_vtx_n_segs;
	  std::vector<float> *lol_2_v_energy;
	  std::vector<float> *lol_2_v_shower_main_length;
	  std::vector<float> *lol_2_v_flag_dir_weak;

	  float lol_3_flag;
	  float lol_3_angle_beam;
	  float lol_3_n_valid_tracks;
	  float lol_3_min_angle;
	  float lol_3_vtx_n_segs;
	  float lol_3_energy;
	  float lol_3_shower_main_length;
	  float lol_3_n_out;
	  float lol_3_n_sum;
  };
  void SetLowEoverlap(LowEoverlap);
  const LowEoverlap & GetLowEoverlap() const;

  /// major cosmic tagger
  struct MajorCosmicTagger{
	  // cosmic tagger
	  float cosmict_flag_1; // fiducial volume vertex
	  float cosmict_flag_2;  // single muon
	  float cosmict_flag_3;  // single muon (long)
	  float cosmict_flag_4;  // kinematics muon
	  float cosmict_flag_5; // kinematics muon (long)
	  float cosmict_flag_6; // special ...
	  float cosmict_flag_7;  // muon+ michel
	  float cosmict_flag_8;  // muon + michel + special
	  float cosmict_flag_9;  // this tagger is relevant for nueCC, see "cosmic tagger ones, one case of cosmics ..." (frist one ...)
	  std::vector<float> *cosmict_flag_10;  // front upstream (dirt)
	  float cosmict_flag;

	  // single muon
	  float cosmict_2_filled;
	  float cosmict_2_particle_type;
	  float cosmict_2_n_muon_tracks;
	  float cosmict_2_total_shower_length;
	  float cosmict_2_flag_inside;
	  float cosmict_2_angle_beam;
	  float cosmict_2_flag_dir_weak;
	  float cosmict_2_dQ_dx_end;
	  float cosmict_2_dQ_dx_front;
	  float cosmict_2_theta;
	  float cosmict_2_phi;
	  float cosmict_2_valid_tracks;

	  // signel muon (long)
	  float cosmict_3_filled;
	  float cosmict_3_flag_inside;
	  float cosmict_3_angle_beam;
	  float cosmict_3_flag_dir_weak;
	  float cosmict_3_dQ_dx_end;
	  float cosmict_3_dQ_dx_front;
	  float cosmict_3_theta;
	  float cosmict_3_phi;
	  float cosmict_3_valid_tracks;

	  // kinematics muon
	  float cosmict_4_filled;
	  float cosmict_4_flag_inside;
	  float cosmict_4_angle_beam;
	  float cosmict_4_connected_showers;  // need to be careful about the nueCC ...

	  // kinematics muon (long)
	  float cosmict_5_filled;
	  float cosmict_5_flag_inside;
	  float cosmict_5_angle_beam;
	  float cosmict_5_connected_showers;

	  // special
	  float cosmict_6_filled;
	  float cosmict_6_flag_dir_weak;
	  float cosmict_6_flag_inside;
	  float cosmict_6_angle;

	  // muon + michel
	  float cosmict_7_filled;
	  float cosmict_7_flag_sec;
	  float cosmict_7_n_muon_tracks;
	  float cosmict_7_total_shower_length;
	  float cosmict_7_flag_inside;
	  float cosmict_7_angle_beam;
	  float cosmict_7_flag_dir_weak;
	  float cosmict_7_dQ_dx_end;
	  float cosmict_7_dQ_dx_front;
	  float cosmict_7_theta;
	  float cosmict_7_phi;

	  // muon + michel + special
	  float cosmict_8_filled;
	  float cosmict_8_flag_out;
	  float cosmict_8_muon_length;
	  float cosmict_8_acc_length;

	  // front upstream (dirt)
	  std::vector<float> *cosmict_10_flag_inside;
	  std::vector<float> *cosmict_10_vtx_z;
	  std::vector<float> *cosmict_10_flag_shower;
	  std::vector<float> *cosmict_10_flag_dir_weak;
	  std::vector<float> *cosmict_10_angle_beam;
	  std::vector<float> *cosmict_10_length;
  };
  void SetMajorCosmicTagger(MajorCosmicTagger);
  const MajorCosmicTagger & GetMajorCosmicTagger() const;

  struct NumuCCTagger{
	  // numu tagger
	  float numu_cc_flag;

	  // single muon connected to neutrino vertex
	  std::vector<float> *numu_cc_flag_1;
	  std::vector<float> *numu_cc_1_particle_type;
	  std::vector<float> *numu_cc_1_length;
	  std::vector<float> *numu_cc_1_medium_dQ_dx;
	  std::vector<float> *numu_cc_1_dQ_dx_cut;
	  std::vector<float> *numu_cc_1_direct_length;
	  std::vector<float> *numu_cc_1_n_daughter_tracks;
	  std::vector<float> *numu_cc_1_n_daughter_all;

	  // long muon connected to neutrino vertex
	  std::vector<float> *numu_cc_flag_2;
	  std::vector<float> *numu_cc_2_length;
	  std::vector<float> *numu_cc_2_total_length;
	  std::vector<float> *numu_cc_2_n_daughter_tracks;
	  std::vector<float> *numu_cc_2_n_daughter_all;

	  // any muon ...
	  float numu_cc_flag_3;
	  float numu_cc_3_particle_type;
	  float numu_cc_3_max_length;
	  float numu_cc_3_acc_track_length;
	  float numu_cc_3_max_length_all;
	  float numu_cc_3_max_muon_length;
	  float numu_cc_3_n_daughter_tracks;
	  float numu_cc_3_n_daughter_all;
  };
  void SetNumuCCTagger(NumuCCTagger);
  const NumuCCTagger & GetNumuCCTagger() const;

  struct BDTscores{
	  // numu_BDTs
	  float cosmict_2_4_score;
	  float cosmict_3_5_score;
	  float cosmict_6_score;
	  float cosmict_7_score;
	  float cosmict_8_score;
	  // vector ...
	  float cosmict_10_score;
	  // vector
	  float numu_1_score;
	  float numu_2_score;
	  // scalar
	  float numu_3_score;
	  // total one
	  float cosmict_score;
	  float numu_score;
	  // nue BDTs
	  float mipid_score;
	  float gap_score;
	  float hol_lol_score;
	  float cme_anc_score;
	  float mgo_mgt_score;
	  float br1_score;
	  float br3_score;
	  float br3_3_score;
	  float br3_5_score;
	  float br3_6_score;
	  float stemdir_br2_score;
	  float trimuon_score;
	  float br4_tro_score;
	  float mipquality_score;
	  float pio_1_score;
	  float pio_2_score;
	  float stw_spt_score;
	  float vis_1_score;
	  float vis_2_score;
	  float stw_2_score;
	  float stw_3_score;
	  float stw_4_score;
	  float sig_1_score;
	  float sig_2_score;
	  float lol_1_score;
	  float lol_2_score;
	  float tro_1_score;
	  float tro_2_score;
	  float tro_4_score;
	  float tro_5_score;
	  float nue_score;
  };
  void SetBDTscores(BDTscores);
  const BDTscores & GetBDTscores() const;

  private:
  SPID _SPID_;
  SPSHWID1 _SPSHWID1_;
  SPSHWID2 _SPSHWID2_;
  SPPi0Tagger1 _SPPi0Tagger1_;
	SPLowEMichel _SPLowEMichel_;
	SPBadReco1 _SPBadReco1_;
	SPBadReco2 _SPBadReco2_;
	SPBadReco3 _SPBadReco3_;
	SPBadReco4 _SPBadReco4_;
	SPHighEoverlap _SPHighEoverlap_;
	SPLowEoverlap _SPLowEoverlap_;
	CosmicTagger _CosmicTagger_;
	GapID _GapID_;
	MipCheck _MipCheck_;
	MipID1 _MipID1_;
	MipID2 _MipID2_;
 	Pi0Tagger1 _Pi0Tagger1_;
	Pi0Tagger2 _Pi0Tagger2_;
	MultiGamma1 _MultiGamma1_;
	MultiGamma2 _MultiGamma2_;
	SingleGamma1 _SingleGamma1_;
	SingleGamma2 _SingleGamma2_;
	StemLen _StemLen_;
	LowEMichel _LowEMichel_;
	BrokenMuon _BrokenMuon_;
	MuEnergy _MuEnergy_;
	ShowerAngle _ShowerAngle_;
	BadStem _BadStem_;
	VtxInShw _VtxInShw_;
	BadReco1 _BadReco1_;
	BadReco2 _BadReco2_;
	BadReco3 _BadReco3_;
	BadReco4 _BadReco4_;
	TrackOverCluster _TrackOverCluster_;
	HighEoverlap _HighEoverlap_;
	LowEoverlap _LowEoverlap_;
	MajorCosmicTagger _MajorCosmicTagger_;
	NumuCCTagger _NumuCCTagger_;
	BDTscores _BDTscores_;
  };
}

#endif
