<?php

function paragraphs_extra_bundle_info_modal_attach(&$element, $context) {
  $attached = &drupal_static(__FUNCTION__);

  if (!$attached) {
    $element['#attached']['js'][] = drupal_get_path('module', 'paragraphs_extra') . '/js/BundleSelect.js';

    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_add_js();

    $classes =array(
      'element-invisible',
      _paragraphs_extra_class('bundle-info', 'modal-trigger'),
    );
    $new_element = array(
      '#markup' => ctools_modal_text_button(
        t('Select Bundle'),
        'ajax/paragraphs-extra/nojs/bundle-select/' . _paragraphs_extra_ajax_api_instance_id($context),
        t('Select Bundle'),
        implode(' ', $classes)
      ),
    );

    $element['add_more']['paragraphs_extra']['bundle_info']['modal'] = $new_element;
    $attached = TRUE;
  }
}

function paragraphs_extra_bundle_info_form_attach(&$form, &$form_state) {
  if (!empty($form_state['build_info']['args'])) {
    $bundle_info = paragraphs_extra_bundle_info_load($form_state['build_info']['args'][0]->bundle);
  }
  else {
    $bundle_info = NULL;
  }

  if (!$bundle_info) {
    $bundle_info = new stdClass();
  }

  $form['paragraphs_extra']['#tree'] = TRUE;

  $form['paragraphs_extra']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('A description to help users select a bundle.'),
    '#default_value' => isset($bundle_info->description) ? $bundle_info->description : '',
  );

  $form['paragraphs_extra']['icon'] = array(
    '#type' => 'managed_file',
    '#title' => t('Icon'),
    '#description' => t('An icon to show for bundle selection.'),
    '#default_value' => isset($bundle_info->icon) ? $bundle_info->icon : '',
  );

  $form['#submit'][] = 'paragraphs_extra_bundle_info_form_submit';
}

function paragraphs_extra_bundle_info_handle_form_submit($form, &$form_state) {
  if (!empty($form_state['build_info']['args'])) {
    $bundle = $form_state['build_info']['args'][0]->bundle;
  }
  else {
    $bundle = $form_state['values']['bundle'];
  }

  $bundle_info = paragraphs_extra_bundle_info_load($bundle);
  if (!$bundle_info) {
    $bundle_info = new stdClass();
    $bundle_info->is_new = TRUE;
  }
  $bundle_info->bundle = $bundle;

  $bundle_info->description = $form_state['values']['paragraphs_extra']['description'];
  $bundle_info->icon = $form_state['values']['paragraphs_extra']['icon'];
  paragraphs_extra_bundle_info_save($bundle, $bundle_info);
}

function paragraphs_extra_bundle_info_load($bundle) {
  return db_select('paragraphs_extra_bundle_info', 'info')
    ->fields('info', array('icon', 'description'))
    ->condition('info.bundle', $bundle)
    ->execute()
    ->fetchObject();
}

function paragraphs_extra_bundle_info_save($bundle, $info) {
  if (isset($info->is_new) && $info->is_new) {
    drupal_write_record('paragraphs_extra_bundle_info', $info);
  }
  else {
    drupal_write_record('paragraphs_extra_bundle_info', $info, 'bundle');
  }
}

function paragraphs_extra_bundle_info_select_page($js, $instance_id) {
  $output = views_embed_view('paragraphs_extra_bundles');
  if ($js) {
    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_render(t('Insert'), $output);
  }
  else {
    return MENU_ACCESS_DENIED;
  }
}
