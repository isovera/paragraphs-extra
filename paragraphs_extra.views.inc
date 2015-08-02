<?php
/**
 * @file
 * Implements extended views support for the paragraphs module.
 */

/**
 * Implements hook_views_data().
 */
function paragraphs_extra_views_data() {
  $info = array();

  $info['paragraphs_bundle']['table']['group'] = t('Paragraphs');

  $info['paragraphs_bundle']['table']['base'] = array(
    'field' => 'bundle',
    'title' => t('Paragraphs bundle'),
    'help' => t('Contains paragraphs bundles available on the system'),
  );

  $info['paragraphs_bundle']['table']['join'] = array(
    'paragraphs_item' => array(
      'left_field' => 'bundle',
      'field' => 'bundle',
    ),
  );

 $info['paragraphs_bundle']['bundle'] = array(
    'title' => t('Bundle (Machine Name)'), 
    'help' => t('The machine name of the paragraphs bundle.'), 
    'relationship' => array(
      'base' => 'paragraphs_extra_bundle_info',
      'base field' => 'bundle',
      'relationship field' => 'bundle',
      'handler' => 'views_handler_relationship',
      'label' => t('Paragraphs Extra Bundle Info'),
    ),
    'field' => array(
      'handler' => 'views_handler_field', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ), 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ), 
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

 $info['paragraphs_bundle']['name'] = array(
    'title' => t('Bundle (Human Name)'), 
    'help' => t('The human readable name of the paragraphs bundle.'), 
    'field' => array(
      'handler' => 'views_handler_field', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ), 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ), 
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $info['paragraphs_bundle']['table']['group'] = t('Paragraphs');

  $info['paragraphs_extra_bundle_info']['table']['base'] = array(
    'field' => 'bundle',
    'title' => t('Paragraphs extra bundle info'),
    'help' => t('Contains paragraphs bundles available on the system'),
  );

  $info['paragraphs_extra_bundle_info']['table']['join'] = array(
    'paragraphs_item' => array(
      'left_field' => 'bundle',
      'field' => 'bundle',
    ),
  );

 $info['paragraphs_extra_bundle_info']['icon'] = array(
    'title' => t('Icon'), 
    'help' => t('The paragraph bundle icon'),
    'group' => t('Paragraphs Extra'),
    'relationship' => array(
      'base' => 'file_managed',
      'base field' => 'fid',
      'relationship field' => 'icon',
      'handler' => 'views_handler_relationship',
      'label' => t('File'),
    ),
    'field' => array(
      'handler' => 'views_handler_field_file', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ), 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ), 
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

 $info['paragraphs_extra_bundle_info']['description'] = array(
    'title' => t('Description'), 
    'help' => t('A description of the paragraph bundle.'), 
    'group' => t('Paragraphs Extra'),
    'field' => array(
      'handler' => 'views_handler_field', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ), 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ), 
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $info;
}
