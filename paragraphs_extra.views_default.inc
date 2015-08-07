<?php
/**
 * @file
 * paragraphs_extra.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function paragraphs_extra_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'paragraphs_extra_bundles';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'paragraphs_bundle';
  $view->human_name = 'Paragraphs Extra: Bundles';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'grid';
  $handler->display->display_options['style_options']['default_row_class'] = FALSE;
  $handler->display->display_options['style_options']['row_class_special'] = FALSE;
  $handler->display->display_options['style_options']['columns'] = '3';
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  /* Relationship: Paragraphs: Bundle (Machine Name) */
  $handler->display->display_options['relationships']['bundle']['id'] = 'bundle';
  $handler->display->display_options['relationships']['bundle']['table'] = 'paragraphs_bundle';
  $handler->display->display_options['relationships']['bundle']['field'] = 'bundle';
  /* Relationship: Paragraphs Extra: Icon */
  $handler->display->display_options['relationships']['icon']['id'] = 'icon';
  $handler->display->display_options['relationships']['icon']['table'] = 'paragraphs_extra_bundle_info';
  $handler->display->display_options['relationships']['icon']['field'] = 'icon';
  $handler->display->display_options['relationships']['icon']['relationship'] = 'bundle';
  /* Field: File: Path */
  $handler->display->display_options['fields']['uri']['id'] = 'uri';
  $handler->display->display_options['fields']['uri']['table'] = 'file_managed';
  $handler->display->display_options['fields']['uri']['field'] = 'uri';
  $handler->display->display_options['fields']['uri']['relationship'] = 'icon';
  $handler->display->display_options['fields']['uri']['label'] = '';
  $handler->display->display_options['fields']['uri']['exclude'] = TRUE;
  $handler->display->display_options['fields']['uri']['alter']['text'] = '<img src="[uri]">';
  $handler->display->display_options['fields']['uri']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['uri']['file_download_path'] = TRUE;
  /* Field: Paragraphs: Bundle (Machine Name) */
  $handler->display->display_options['fields']['bundle']['id'] = 'bundle';
  $handler->display->display_options['fields']['bundle']['table'] = 'paragraphs_bundle';
  $handler->display->display_options['fields']['bundle']['field'] = 'bundle';
  $handler->display->display_options['fields']['bundle']['exclude'] = TRUE;
  /* Field: Paragraphs: Bundle (Human Name) */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'paragraphs_bundle';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = '';
  $handler->display->display_options['fields']['name']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['text'] = '<div data-paragraphs-extra-bundle="[bundle]" class="paragraphs-extra-bundle-option">
  <img class="paragraphs-extra-bundle-option__icon" src="[uri]">
  <span class="paragraphs-extra-bundle-option__name">[name]</span>
</div>';
  $handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
  /* Filter criterion: Paragraphs: Bundle (Human Name) */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'paragraphs_bundle';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  $export['paragraphs_extra_bundles'] = $view;

  return $export;
}
